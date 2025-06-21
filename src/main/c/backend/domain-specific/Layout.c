#include "Layout.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"

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

static gboolean int_equal(gconstpointer v1, gconstpointer v2) {
    return *((int *)v1) == *((int *)v2);
}

// Convert int to pointer for hash table keys
static gpointer int_key(int value) { return GINT_TO_POINTER(value); }

Row *create_row() {
    Row *row = calloc(1, sizeof(Row));
    row->columns = g_hash_table_new_full(int_hash, int_equal, free, NULL);
    row->minCol = INT_MAX;
    row->maxCol = INT_MIN;
    return row;
}

Slide *create_slide() {
    Slide *slide = calloc(1, sizeof(Slide));
    slide->rows = g_hash_table_new_full(int_hash, int_equal, free, (GDestroyNotify)free);
    slide->symbolToObject = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
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
        int row = *((int *)rk->data);
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
    Row *row = g_hash_table_lookup(slide->rows, &obj->row);
    if (!row)
        return;
    g_hash_table_remove(row->columns, &obj->col);
}

void move_object(Slide *slide, PositionedObject *obj, int newRow, int newCol) {
    remove_object(slide, obj);
    obj->row = newRow;
    obj->col = newCol;

    Row *rowPtr = g_hash_table_lookup(slide->rows, &newRow);
    if (!rowPtr) {
        rowPtr = create_row();
        g_hash_table_insert(slide->rows, int_key(newRow), rowPtr);
    }
    g_hash_table_insert(rowPtr->columns, int_key(newCol), obj);

    if (newCol < rowPtr->minCol)
        rowPtr->minCol = newCol;
    if (newCol > rowPtr->maxCol)
        rowPtr->maxCol = newCol;

    update_bounds(slide);
}

void insert_object(Slide *slide, const char *identifier, int row, int col) {
    PositionedObject *obj = calloc(1, sizeof(PositionedObject));
    obj->identifier = strdup(identifier);
    obj->row = row;
    obj->col = col;

    Row *rowPtr = g_hash_table_lookup(slide->rows, &row);
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

void apply_relative_positions(Slide *slide, SlideContent *positions) {
    for (SlideContent *pos = positions; pos != NULL; pos = pos->next) {
        PositionedObject *parent =
            g_hash_table_lookup(slide->symbolToObject, pos->position_items.parent);
        PositionedObject *child =
            g_hash_table_lookup(slide->symbolToObject, pos->position_items.child);
        if (!parent || !child)
            continue;

        int newRow = parent->row;
        int newCol = parent->col;

        switch (pos->position_items.pos) {
        case POS_TOP:
            newRow -= 1;
            break;
        case POS_BOTTOM:
            newRow += 1;
            break;
        case POS_LEFT:
            newCol -= 1;
            break;
        case POS_RIGHT:
            newCol += 1;
            break;
        case POS_TOP_LEFT:
            newRow -= 1;
            newCol -= 1;
            break;
        case POS_TOP_RIGHT:
            newRow -= 1;
            newCol += 1;
            break;
        case POS_BOTTOM_LEFT:
            newRow += 1;
            newCol -= 1;
            break;
        case POS_BOTTOM_RIGHT:
            newRow += 1;
            newCol += 1;
            break;
        }

        move_object(slide, child, newRow, newCol);
    }
}

/* Generate the slide object lists */
SlideList *generateObjects(CompilerState *compilerState) { return NULL; }

/**************************************** DESTRUCTORS*
 * ********************************************/
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
}
