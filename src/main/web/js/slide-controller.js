/**
 * SlideController class
 * Controls the presentation slides and animations
 */
class SlideController {
    /**
     * Create a new SlideController
     */
    constructor() {
        this.slides = document.querySelectorAll('.slide');
        this.slideNumber = document.querySelector('.slide-number');
        this.isAnimating = false;
        this.slideList = new SlideList(Array.from(this.slides));
        this.slideStates = new Map();

        this.initializeSlides();
    }

    /**
     * Initialize the slides and their animations
     */
    initializeSlides() {
        this.slides.forEach(slide => {
            const elements = Array.from(slide.querySelectorAll('.anim'));
            AnimationUtils.initializeElements(elements);
        });

        this.showSlide(this.slideList.getCurrentSlide());
        this.slideList.getCurrentAnimationList()?.reset();
    }

    /**
     * Update the slide number indicator
     */
    updateSlideNumber() {
        this.slideNumber.textContent = `${this.slideList.currentIndex + 1} / ${this.slides.length}`;
    }

    /**
     * Show a slide
     * @param {Element} slide - Slide to show
     */
    showSlide(slide) {
        this.slides.forEach(s => s.classList.remove('active'));
        slide.classList.add('active');
        this.updateSlideNumber();
    }

    /**
     * Save the current state of a slide
     * @param {number} slideIndex - Index of the slide to save
     */
    saveSlideState(slideIndex) {
        const animationList = this.slideList.getAnimationList(slideIndex);
        if (!animationList) return;
        
        const allElements = animationList.getAllElements();
        const visibilityState = new Map();
        
        allElements.forEach(element => {
            const isVisible = element.style.opacity === '1';
            const animation = element.dataset.animation || 'appear';
            visibilityState.set(element, {
                isVisible,
                animation,
                isAnimated: animationList.getAnimatedElements().includes(element)
            });
        });
        
        this.slideStates.set(slideIndex, {
            animatedElements: [...animationList.getAnimatedElements()],
            currentIndex: animationList.currentIndex,
            animationSequence: [...animationList.getAnimationSequence()],
            visibilityState
        });
    }

    /**
     * Restore a slide to its saved state
     * @param {number} slideIndex - Index of the slide to restore
     */
    restoreSlideState(slideIndex) {
        const state = this.slideStates.get(slideIndex);
        if (!state) return;

        const animationList = this.slideList.getAnimationList(slideIndex);
        if (!animationList) return;
        
        animationList.reset();
        
        const allElements = animationList.getAllElements();
        AnimationUtils.makeElementsInvisible(allElements);
        
        if (state.visibilityState) {
            state.visibilityState.forEach((elementState, element) => {
                element.style.opacity = elementState.isVisible ? '1' : '0';
            });
        } else {
            AnimationUtils.makeElementsVisible(state.animatedElements);
        }
        
        animationList.restoreAnimationState(state.animationSequence);
    }

    /**
     * Handle forward navigation
     */
    async handleForward() {
        if (this.isAnimating) return;
        this.isAnimating = true;

        try {
            const currentList = this.slideList.getCurrentAnimationList();
            const currentSlideIndex = this.slideList.currentIndex;
            
            if (currentList?.hasNext()) {
                await this.animateNextElement(currentList);
                this.saveSlideState(currentSlideIndex);
            } else {
                await this.moveToNextSlide(currentSlideIndex);
            }
        } finally {
            this.isAnimating = false;
        }
    }
    
    /**
     * Animate the next element in the current slide
     * @param {AnimationList} currentList - The current animation list
     */
    async animateNextElement(currentList) {
        const element = currentList.next();
        if (!element) return;
        await AnimationUtils.animateElement(element, true);
    }
    
    /**
     * Move to the next slide
     * @param {number} currentSlideIndex - The current slide index
     */
    async moveToNextSlide(currentSlideIndex) {
        this.saveSlideState(currentSlideIndex);
        
        if (!this.slideList.next()) return;
        
        const currentSlide = this.slideList.getSlide(currentSlideIndex);
        const nextSlide = this.slideList.getCurrentSlide();
        const newSlideIndex = this.slideList.currentIndex;
        
        await SlideTransition.transition(currentSlide, nextSlide, true);
        
        this.updateSlideNumber();
        
        if (this.slideStates.has(newSlideIndex)) {
            this.restoreSlideState(newSlideIndex);
        } else {
            this.initializeNewSlide();
        }
    }
    
    /**
     * Initialize a new slide that hasn't been visited before
     */
    initializeNewSlide() {
        const newList = this.slideList.getCurrentAnimationList();
        if (!newList) return;
        
        const newElements = newList.getAllElements();
        AnimationUtils.initializeElements(newElements);
    }

    /**
     * Handle backward navigation
     */
    async handleBackward() {
        if (this.isAnimating) return;
        this.isAnimating = true;

        try {
            const currentList = this.slideList.getCurrentAnimationList();
            
            if (currentList?.hasPrevious()) {
                await this.animatePreviousElement(currentList);
                this.saveSlideState(this.slideList.currentIndex);
            } else {
                await this.moveToPreviousSlide();
            }
        } finally {
            this.isAnimating = false;
        }
    }
    
    /**
     * Animate the previous element in the current slide
     * @param {AnimationList} currentList - The current animation list
     */
    async animatePreviousElement(currentList) {
        const elementToHide = currentList.previous();
        if (!elementToHide) return;
        AnimationUtils.removeAnimationClasses(elementToHide);
        await new Promise(resolve => setTimeout(resolve, 10));
        await AnimationUtils.animateElement(elementToHide, false);
    }
    
    /**
     * Move to the previous slide
     */
    async moveToPreviousSlide() {
        const currentSlideIndex = this.slideList.currentIndex;
        this.saveSlideState(currentSlideIndex);
        
        if (!this.slideList.previous()) return;
        
        const currentSlide = this.slideList.getSlide(currentSlideIndex);
        const prevSlide = this.slideList.getCurrentSlide();
        const previousSlideIndex = this.slideList.currentIndex;
        
        await SlideTransition.transition(currentSlide, prevSlide, false);
        
        this.updateSlideNumber();
        
        if (this.slideStates.has(previousSlideIndex)) {
            this.restoreSlideState(previousSlideIndex);
        } else {
            const previousList = this.slideList.getCurrentAnimationList();
            const previousElements = previousList?.getAllElements();
            if (previousElements) {
                AnimationUtils.makeElementsVisible(previousElements);
            }
        }
    }
} 