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
        
        const allElements = animationList.getAllElements();
        const visibilityState = new Map();
        
        allElements.forEach(element => {
            const isVisible = element.style.opacity === '1';
            const animation = element.dataset.animation || 'appear';
            visibilityState.set(element, {
                isVisible: isVisible,
                animation: animation,
                isAnimated: animationList.getAnimatedElements().includes(element)
            });
        });
        
        this.slideStates.set(slideIndex, {
            animatedElements: [...animationList.getAnimatedElements()],
            currentIndex: animationList.currentIndex,
            animationSequence: [...animationList.getAnimationSequence()],
            visibilityState: visibilityState
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
                if (elementState.isVisible) {
                    element.style.opacity = '1';
                    if (elementState.animation === 'barrel-roll') {
                        element.classList.add('barrel-roll-completed');
                    }
                } else {
                    element.style.opacity = '0';
                }
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

        const currentList = this.slideList.getCurrentAnimationList();
        const currentSlideIndex = this.slideList.currentIndex;
        
        if (currentList.hasNext()) {
            const element = currentList.next();
            const animation = element.dataset.animation || 'appear';
            
            if (animation === 'disappear') {
                await AnimationUtils.animateElement(element, false);
            } else {
                await AnimationUtils.animateElement(element, true);
            }
            
            this.saveSlideState(currentSlideIndex);
        } else {
            this.saveSlideState(currentSlideIndex);
            
            if (this.slideList.next()) {
                const oldElements = currentList.getAllElements();
                AnimationUtils.makeElementsInvisible(oldElements);
                
                this.showSlide(this.slideList.getCurrentSlide());
                const newSlideIndex = this.slideList.currentIndex;
                
                if (this.slideStates.has(newSlideIndex)) {
                    this.restoreSlideState(newSlideIndex);
                } else {
                    const newList = this.slideList.getCurrentAnimationList();
                    const newElements = newList.getAllElements();
                    
                    newElements.forEach(element => {
                        const animation = element.dataset.animation || 'appear';
                        
                        if (animation === 'barrel-roll') {
                            element.style.opacity = '1';
                            element.classList.add('barrel-roll-completed');
                        } else if (animation === 'disappear') {
                            element.style.opacity = '1';
                        } else if (animation !== 'appear') {
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
            const elementToHide = currentList.previous();
            const animation = elementToHide.dataset.animation || 'appear';
            
            if (animation === 'disappear') {
                await AnimationUtils.animateElement(elementToHide, true);
            } else if (animation === 'barrel-roll') {
                await AnimationUtils.animateElement(elementToHide, false);
                elementToHide.style.opacity = '1';
            } else {
                await AnimationUtils.animateElement(elementToHide, false);
            }
            
            const currentSlideIndex = this.slideList.currentIndex;
            this.saveSlideState(currentSlideIndex);
        } else {
            const currentSlideIndex = this.slideList.currentIndex;
            this.saveSlideState(currentSlideIndex);
            
            if (this.slideList.previous()) {
                const currentElements = currentList.getAllElements();
                AnimationUtils.makeElementsInvisible(currentElements);
                
                this.showSlide(this.slideList.getCurrentSlide());
                const previousSlideIndex = this.slideList.currentIndex;
                
                if (this.slideStates.has(previousSlideIndex)) {
                    this.restoreSlideState(previousSlideIndex);
                } else {
                    const previousList = this.slideList.getCurrentAnimationList();
                    const previousElements = previousList.getAllElements();
                    AnimationUtils.makeElementsVisible(previousElements);
                }
            }
        }

        this.isAnimating = false;
    }
} 