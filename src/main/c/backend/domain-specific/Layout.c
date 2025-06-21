#include "Layout.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include <glib.h>

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeObjectsModule() { _logger = createLogger("Objects"); }

void shutdownObjectsModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

// Hash and equality for int* keys
static guint int_hash(gconstpointer v) { return GPOINTER_TO_INT(v); }

static gboolean int_equal(gconstpointer v1, gconstpointer v2) { return v1 == v2; }

// Convert int to pointer for hash table keys
static gpointer int_key(int value) { return GINT_TO_POINTER(value); }

static void destroy_positioned_object(gpointer data) {
    PositionedObject *obj = data;
    if (obj) {
        free(obj->identifier);
        free(obj);
    }
}

Row *create_row() {
    Row *row = calloc(1, sizeof(Row));
    row->columns = g_hash_table_new_full(int_hash, int_equal, NULL, NULL);
    row->minCol = INT_MAX;
    row->maxCol = INT_MIN;
    return row;
}

static void destroy_row(gpointer data) {
    Row *row = data;
    g_hash_table_destroy(row->columns);
    free(row);
}

Slide *create_slide() {
    logInformation(_logger, "Creating a new slide");
    Slide *slide = calloc(1, sizeof(Slide));
    slide->rows = g_hash_table_new_full(int_hash, int_equal, NULL, (GDestroyNotify)destroy_row);
    slide->symbolToObject =
        g_hash_table_new_full(g_str_hash, g_str_equal, free, destroy_positioned_object);
    slide->minRow = INT_MAX;
    slide->maxRow = INT_MIN;
    slide->minCol = INT_MAX;
    slide->maxCol = INT_MIN;
    return slide;
}

void update_bounds(Slide *slide) {
    slide->minRow = INT_MAX;
    slide->maxRow = INT_MIN;
    slide->minCol = INT_MAX;
    slide->maxCol = INT_MIN;

    GList *rowKeys = g_hash_table_get_keys(slide->rows);
    for (GList *rk = rowKeys; rk != NULL; rk = rk->next) {
        int row = GPOINTER_TO_INT(rk->data);
        Row *rowPtr = g_hash_table_lookup(slide->rows, rk->data);
        if (!rowPtr)
            continue;

        if (row < slide->minRow)
            slide->minRow = row;
        if (row > slide->maxRow)
            slide->maxRow = row;
        if (rowPtr->minCol < slide->minCol)
            slide->minCol = rowPtr->minCol;
        if (rowPtr->maxCol > slide->maxCol)
            slide->maxCol = rowPtr->maxCol;
    }
    g_list_free(rowKeys);
}

void remove_object(Slide *slide, PositionedObject *obj) {
    Row *row = g_hash_table_lookup(slide->rows, int_key(obj->row));
    if (!row)
        return;
    g_hash_table_steal(row->columns, int_key(obj->col));
}

void move_object(Slide *slide, PositionedObject *obj, int newRow, int newCol) {
    // Check for a collision at the destination.
    Row *destRowPtr = g_hash_table_lookup(slide->rows, int_key(newRow));
    if (destRowPtr) {
        PositionedObject *occupant = g_hash_table_lookup(destRowPtr->columns, int_key(newCol));
        // If the spot is occupied by a *different* object, cancel the move.
        if (occupant && occupant != obj) {
            logWarning(_logger,
                       "Cannot move object '%s' to (%d, %d). Position is already occupied by "
                       "'%s'. The move is cancelled.",
                       obj->identifier, newRow, newCol, occupant->identifier);
            return;
        }
    }

    remove_object(slide, obj);
    obj->row = newRow;
    obj->col = newCol;

    if (!destRowPtr) {
        destRowPtr = create_row();
        g_hash_table_insert(slide->rows, int_key(newRow), destRowPtr);
    }
    g_hash_table_insert(destRowPtr->columns, int_key(newCol), obj);

    if (newCol < destRowPtr->minCol)
        destRowPtr->minCol = newCol;
    if (newCol > destRowPtr->maxCol)
        destRowPtr->maxCol = newCol;

    update_bounds(slide);
}

void insert_object(Slide *slide, const char *identifier, int row, int col) {
    PositionedObject *obj = calloc(1, sizeof(PositionedObject));
    obj->identifier = strdup(identifier);
    obj->row = row;
    obj->col = col;

    Row *rowPtr = g_hash_table_lookup(slide->rows, int_key(row));
    if (!rowPtr) {
        rowPtr = create_row();
        g_hash_table_insert(slide->rows, int_key(row), rowPtr);
    }
    g_hash_table_insert(rowPtr->columns, int_key(col), obj);

    if (col < rowPtr->minCol)
        rowPtr->minCol = col;
    if (col > rowPtr->maxCol)
        rowPtr->maxCol = col;

    g_hash_table_insert(slide->symbolToObject, strdup(identifier), obj);
    update_bounds(slide);
}

void populate_slide_with_content(Slide *slide, SlideContent *contentList) {
    int currentRow = 0;
    for (SlideContent *content = contentList; content != NULL; content = content->next) {
        if (content->type == SLIDE_CONTENT_ADD && content->add.identifier != NULL) {
            insert_object(slide, content->add.identifier, currentRow++, 0);
        }
    }
}

/********************* functions to detect cycles + rebuild grid based on new info **************/
typedef enum { NODE_UNVISITED, NODE_VISITING, NODE_VISITED } VisitState;

typedef struct {
    PositionedObject *parent;
    Position pos_type;
    VisitState visit_state;
    int final_row;
    int final_col;
    gboolean is_resolved;
} DependencyInfo;

static gboolean resolve_object_position(PositionedObject *obj, GHashTable *dep_graph);

void apply_relative_positions(Slide *slide, SlideContent *positions) {

    GHashTable *dep_graph = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);

    for (SlideContent *pos = positions; pos != NULL; pos = pos->next) {
        if (pos->type != SLIDE_CONTENT_DOUBLE_POS)
            continue;

        PositionedObject *child =
            g_hash_table_lookup(slide->symbolToObject, pos->position_items.child);
        PositionedObject *parent =
            g_hash_table_lookup(slide->symbolToObject, pos->position_items.parent);

        if (!parent || !child)
            continue;

        if (g_hash_table_contains(dep_graph, child)) {
            logWarning(_logger,
                       "Object '%s' already has a relative position defined. Ignoring new rule.",
                       child->identifier);
            continue;
        }

        DependencyInfo *info = calloc(1, sizeof(DependencyInfo));
        info->parent = parent;
        info->pos_type = pos->position_items.pos;
        info->visit_state = NODE_UNVISITED;
        info->is_resolved = FALSE;
        g_hash_table_insert(dep_graph, child, info);
    }

    // If we detect
    GList *children = g_hash_table_get_keys(dep_graph);
    gboolean cycle_detected = FALSE;
    for (GList *l = children; l != NULL; l = l->next) {
        PositionedObject *child = l->data;
        if (!resolve_object_position(child, dep_graph)) {
            logCritical(_logger,
                        "A cycle was detected in the positioning rules for slide '%s'. Aborting "
                        "layout process for this slide.",
                        slide->identifier);
            cycle_detected = TRUE;
            break;
        }
    }
    g_list_free(children);

    if (cycle_detected) {
        // cleanup
        g_hash_table_destroy(dep_graph);
        return;
    }
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, dep_graph);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        PositionedObject *obj = key;
        DependencyInfo *info = value;
        obj->row = info->final_row;
        obj->col = info->final_col;
    }

    // grid rebuilding
    GList *all_rows = g_hash_table_get_keys(slide->rows);
    for (GList *l = all_rows; l != NULL; l = l->next) {
        Row *row = g_hash_table_lookup(slide->rows, l->data);
        g_hash_table_remove_all(row->columns);
    }
    g_hash_table_remove_all(slide->rows);
    g_list_free(all_rows);

    GList *all_objects = g_hash_table_get_values(slide->symbolToObject);
    for (GList *l = all_objects; l != NULL; l = l->next) {
        PositionedObject *obj = l->data;
        Row *rowPtr = g_hash_table_lookup(slide->rows, int_key(obj->row));
        if (!rowPtr) {
            rowPtr = create_row();
            g_hash_table_insert(slide->rows, int_key(obj->row), rowPtr);
        }
        g_hash_table_insert(rowPtr->columns, int_key(obj->col), obj);
    }
    g_list_free(all_objects);

    update_bounds(slide);
    g_hash_table_destroy(dep_graph);
}

static gboolean resolve_object_position(PositionedObject *obj, GHashTable *dep_graph) {
    DependencyInfo *info = g_hash_table_lookup(dep_graph, obj);
    if (!info) {
        // this object is not a child in any rule, so its position is its initial one
        return TRUE;
    }

    if (info->is_resolved) {
        return TRUE;
    }
    if (info->visit_state == NODE_VISITING) {
        // Cycle detected
        return FALSE;
    }

    info->visit_state = NODE_VISITING;

    // solve parent position FIRST (recursively)
    if (!resolve_object_position(info->parent, dep_graph)) {
        return FALSE;
    }

    DependencyInfo *parent_info = g_hash_table_lookup(dep_graph, info->parent);
    int parent_row, parent_col;
    if (parent_info) {
        parent_row = parent_info->final_row;
        parent_col = parent_info->final_col;
    } else {
        // Parent was not a child, so use its initial position
        parent_row = info->parent->row;
        parent_col = info->parent->col;
    }

    info->final_row = parent_row;
    info->final_col = parent_col;
    switch (info->pos_type) {
    case POS_TOP:
        info->final_row -= 1;
        break;
    case POS_BOTTOM:
        info->final_row += 1;
        break;
    case POS_LEFT:
        info->final_col -= 1;
        break;
    case POS_RIGHT:
        info->final_col += 1;
        break;
    case POS_TOP_LEFT:
        info->final_row -= 1;
        info->final_col -= 1;
        break;
    case POS_TOP_RIGHT:
        info->final_row -= 1;
        info->final_col += 1;
        break;
    case POS_BOTTOM_LEFT:
        info->final_row += 1;
        info->final_col -= 1;
        break;
    case POS_BOTTOM_RIGHT:
        info->final_row += 1;
        info->final_col += 1;
        break;
    }
    logInformation(
        _logger,
        "Object '%s' resolved to position (%d, %d) based on parent '%s' at (%d, %d) with position "
        "type %d",
        obj->identifier, info->final_row, info->final_col, info->parent->identifier, parent_row,
        parent_col, info->pos_type);
    info->visit_state = NODE_VISITED;
    info->is_resolved = TRUE;
    return TRUE;
}

/*******************************************************************************************/

/* Generate the slide objects for positioning */
SlideList *generateObjects(Program *program) {
    /* I iterate through all slides.. */
    StructureDefinition *structureDefinition = program->structure_definitions;

    SlideList *list = calloc(1, sizeof(SlideList));
    list->head = NULL;
    list->tail = NULL;
    Slide *currentSlide = NULL;
    for (StructureDefinition *structure = structureDefinition; structure != NULL;
         structure = structure->next) {
        logInformation(_logger, "Processing structure: %s", structure->identifier);
        if (structure->content == NULL)
            continue;
        // Create a new slide
        Slide *slide = create_slide();
        slide->identifier =
            structure->identifier; // I dont free this because the pointer is owned by the AST

        // Prepend the new slide to the list. Because the parser gives us a reversed
        // list of structures, prepending here puts them back in the correct order.
        if (list->head == NULL) {
            list->head = slide;
            list->tail = slide;
        } else {
            slide->next = list->head;
            list->head = slide;
        }
        populate_slide_with_content(slide, structure->content);
        apply_relative_positions(slide, structure->positions);
    }
    for (Slide *slide = list->head; slide != NULL; slide = slide->next) {
        logInformation(_logger, "Slide '%s' has bounds: [%d, %d] x [%d, %d]", slide->identifier,
                       slide->minRow, slide->maxRow, slide->minCol, slide->maxCol);
    }
    return list;
}

/**************************************** DESTRUCTORS *********************************************/
void destroySlides(Slide *slide) {
    if (!slide)
        return;

    destroySlides(slide->next);

    if (slide->rows) {
        g_hash_table_destroy(slide->rows);
        slide->rows = NULL;
    }
    if (slide->symbolToObject) {
        g_hash_table_destroy(slide->symbolToObject);
        slide->symbolToObject = NULL;
    }

    g_free(slide);
};

void destroyObjects(SlideList *list) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

    if (!list)
        return;

    destroySlides(list->head);
    list->head = NULL;
    list->tail = NULL;
    free(list);
}
