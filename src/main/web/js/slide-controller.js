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
        // Set initial opacity for animation elements
        this.slides.forEach(slide => {
            const elements = Array.from(slide.querySelectorAll('.anim'));
            AnimationUtils.initializeElements(elements);
        });

        // Show the first slide
        this.showSlide(this.slideList.getCurrentSlide());
        const firstList = this.slideList.getCurrentAnimationList();
        firstList.reset();
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
        
        this.slideStates.set(slideIndex, {
            animatedElements: [...animationList.getAnimatedElements()],
            currentIndex: animationList.currentIndex,
            animationSequence: [...animationList.getAnimationSequence()]
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
        
        // Make all elements initially invisible
        const allElements = animationList.getAllElements();
        AnimationUtils.makeElementsInvisible(allElements);
        
        // Make previously animated elements visible
        AnimationUtils.makeElementsVisible(state.animatedElements);
        
        animationList.restoreAnimationState(state.animationSequence);
    }

    /**
     * Handle forward navigation
     */
    async handleForward() {
        if (this.isAnimating) return;
        this.isAnimating = true;

        const currentList = this.slideList.getCurrentAnimationList();
        const currentSlideIndex = this.slideList.currentIndex;
        
        if (currentList.hasNext()) {
            // Animate the next element in the current slide
            const element = currentList.next();
            await AnimationUtils.animateElement(element, true);
            
            // Save state after each animation
            this.saveSlideState(currentSlideIndex);
        } else {
            // Save the state of the current slide before moving
            this.saveSlideState(currentSlideIndex);
            
            // Move to the next slide if available
            if (this.slideList.next()) {
                // Hide all elements in the current slide without animations
                const oldElements = currentList.getAllElements();
                AnimationUtils.makeElementsInvisible(oldElements);
                
                // Show the next slide
                this.showSlide(this.slideList.getCurrentSlide());
                const newSlideIndex = this.slideList.currentIndex;
                
                // If we've been to this slide before, restore its state
                if (this.slideStates.has(newSlideIndex)) {
                    this.restoreSlideState(newSlideIndex);
                } else {
                    // Initialize new slide elements
                    const newList = this.slideList.getCurrentAnimationList();
                    const newElements = newList.getAllElements();
                    
                    newElements.forEach(element => {
                        if (element.dataset.animation === 'barrel-roll') {
                            element.style.opacity = '1';
                            element.classList.add('barrel-roll-completed');
                        } else if (element.dataset.animation !== 'appear') {
                            element.style.opacity = '1';
                        } else {
                            element.style.opacity = '0';
                        }
                    });
                }
            }
        }

        this.isAnimating = false;
    }

    /**
     * Handle backward navigation
     */
    async handleBackward() {
        if (this.isAnimating) return;
        this.isAnimating = true;

        const currentList = this.slideList.getCurrentAnimationList();
        
        if (currentList.hasPrevious()) {
            // Hide the last animated element in the current slide with reverse animation
            const elementToHide = currentList.previous();
            const isBarrelRoll = elementToHide.dataset.animation === 'barrel-roll';
            
            await AnimationUtils.animateElement(elementToHide, false);
            
            // Ensure barrel-roll elements remain visible after reverse animation
            if (isBarrelRoll) {
                elementToHide.style.opacity = '1';
            }
            
            // Save state after each animation reversal
            const currentSlideIndex = this.slideList.currentIndex;
            this.saveSlideState(currentSlideIndex);
        } else {
            // Save the current state before moving back
            const currentSlideIndex = this.slideList.currentIndex;
            this.saveSlideState(currentSlideIndex);
            
            // Move to the previous slide if available
            if (this.slideList.previous()) {
                // Hide current slide elements without animations
                const currentElements = currentList.getAllElements();
                AnimationUtils.makeElementsInvisible(currentElements);
                
                // Show the previous slide
                this.showSlide(this.slideList.getCurrentSlide());
                const previousSlideIndex = this.slideList.currentIndex;
                
                // Restore the previous slide to its exact state when we left it
                if (this.slideStates.has(previousSlideIndex)) {
                    this.restoreSlideState(previousSlideIndex);
                } else {
                    // If no saved state, show all elements (fallback)
                    const previousList = this.slideList.getCurrentAnimationList();
                    const previousElements = previousList.getAllElements();
                    AnimationUtils.makeElementsVisible(previousElements);
                }
            }
        }

        this.isAnimating = false;
    }
} 