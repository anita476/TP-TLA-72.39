/**
 * SlideList class
 * Manages a list of slides and their associated animation lists
 */
class SlideList {
    /**
     * Create a new SlideList
     * @param {Array} slides - Array of slide DOM elements
     */
    constructor(slides) {
        this.slides = slides || [];
        this.currentIndex = 0;
        this.visitedSlides = [0];
        this.animationLists = new Map();
        
        this.initializeAnimationLists();
    }
    
    /**
     * Initialize animation lists for all slides
     */
    initializeAnimationLists() {
        this.slides.forEach((slide, index) => {
            const elements = Array.from(slide.querySelectorAll('.anim'));
            this.animationLists.set(index, new AnimationList(elements));
        });
    }

    /**
     * Get the current slide
     * @returns {Element} The current slide element
     */
    getCurrentSlide() {
        return this.slides[this.currentIndex];
    }
    
    /**
     * Get a slide by index
     * @param {number} index - Index of the slide to get
     * @returns {Element} The slide element at the specified index
     */
    getSlide(index) {
        return this.slides[index];
    }

    /**
     * Get the animation list for the current slide
     * @returns {AnimationList} The current animation list
     */
    getCurrentAnimationList() {
        return this.animationLists.get(this.currentIndex);
    }

    /**
     * Get the animation list for a specific slide
     * @param {number} index - Index of the slide
     * @returns {AnimationList} The animation list for the specified slide
     */
    getAnimationList(index) {
        return this.animationLists.get(index);
    }

    /**
     * Move to the next slide
     * @returns {boolean} True if moved to next slide, false if already at last slide
     */
    next() {
        if (this.currentIndex >= this.slides.length - 1) return false;
        this.currentIndex++;
        if (this.visitedSlides.indexOf(this.currentIndex) === -1 || 
            this.visitedSlides[this.visitedSlides.length - 1] !== this.currentIndex) {
            this.visitedSlides.push(this.currentIndex);
        }
        return true;
    }

    /**
     * Move to the previous slide
     * @returns {boolean} True if moved to previous slide, false if already at first slide
     */
    previous() {
        if (this.visitedSlides.length <= 1) return false;
        if (this.visitedSlides[this.visitedSlides.length - 1] === this.currentIndex) {
            this.visitedSlides.pop();
        }
        this.currentIndex = this.visitedSlides[this.visitedSlides.length - 1];
        return true;
    }

    /**
     * Get all slides
     * @returns {Array} All slides
     */
    getAllSlides() {
        return this.slides;
    }

    /**
     * Reset the animation state for a specific slide
     * @param {number} index - Index of the slide to reset
     */
    resetSlideAnimations(index) {
        const animationList = this.animationLists.get(index);
        if (animationList) {
            animationList.reset();
        }
    }

    /**
     * Check if the current slide is the first slide
     * @returns {boolean} True if on the first slide
     */
    isFirstSlide() {
        return this.currentIndex === 0;
    }

    /**
     * Check if the current slide is the last slide
     * @returns {boolean} True if on the last slide
     */
    isLastSlide() {
        return this.currentIndex === this.slides.length - 1;
    }
} 