const Controller = {
  isAnimating: false,
  sequences: new Map(),
  repeatCount: new Map(),
  
  /**
   * Initialize the controller
   * @param {Object} core - Core module reference
   */
  init(core) {
    this.core = core;
    this.slides = core.get('slides');
    this.animation = core.get('animation');
    this.transition = core.get('transition');
    this.ui = core.get('ui');
    
    this.slides.slides.forEach((slide, index) => {
      const elements = this.slides.getAnimationElements(index);
      this.sequences.set(index, new AnimationSequence(elements));
      this.repeatCount.set(index, 1);
    });
    
    document.addEventListener('keydown', this.handleKeydown.bind(this));
    this.setupImageErrorHandling();
  },
  
  /**
   * Handle keyboard navigation
   * @param {Event} e - Keyboard event
   */
  handleKeydown(e) {
    switch (e.key) {
      case 'ArrowRight':
      case ' ':
        this.forward();
        break;
      case 'ArrowLeft':
        this.backward();
        break;
    }
  },
  
  /**
   * Set up image error handling
   */
  setupImageErrorHandling() {
    document.querySelectorAll('img').forEach(img => {
      img.onerror = function() {
        this.style.display = 'none';
        const errorText = document.createElement('p');
        errorText.textContent = `(Place your image here: ${this.getAttribute('src')})`;
        errorText.style.color = '#666';
        errorText.style.fontStyle = 'italic';
        this.parentNode.insertBefore(errorText, this.nextSibling);
      };
    });
  },
  
  /**
   * Handle forward navigation
   */
  async forward() {
    if (this.isAnimating) return;
    this.isAnimating = true;
    
    try {
      const currentIndex = this.slides.currentIndex;
      const sequence = this.sequences.get(currentIndex);
      
      if (sequence?.hasNext()) {
        await this.animateNext(sequence);
        this.saveState(currentIndex);
      } else {
        const maxRepeats = this.slides.getRepeats(currentIndex);
        const currentRepeat = this.repeatCount.get(currentIndex) || 1;
        
        if (currentRepeat < maxRepeats) {
          this.repeatCount.set(currentIndex, currentRepeat + 1);
          
          sequence.reset();
          this.animation.resetElements(this.slides.getAnimationElements(currentIndex));
          
          await this.animateNext(sequence);
          this.saveState(currentIndex);
        } else {
          this.repeatCount.set(currentIndex, 1);
          await this.moveToNextSlide();
        }
      }
    } finally {
      this.isAnimating = false;
    }
  },
  
  /**
   * Animate the next element or animation
   * @param {AnimationSequence} sequence - Animation sequence
   */
  async animateNext(sequence) {
    const result = sequence.next();
    if (!result) return;
    
    const { element, animation, isNewElement } = result;
    
    if (isNewElement) {
      this.animation.removeClasses(element);
      const type = this.animation.getType(animation);
      if (type === 'show') element.style.opacity = '0';
      else if (type) element.style.opacity = '1';
    }
    
    await this.animation.animateElement(element, animation, true);
  },
  
  /**
   * Move to the next slide
   */
  async moveToNextSlide() {
    const currentIndex = this.slides.currentIndex;
    this.saveState(currentIndex);
    
    if (!this.slides.next()) return;
    
    const fromSlide = this.slides.get(currentIndex);
    const toSlide = this.slides.current();
    const newIndex = this.slides.currentIndex;
    
    await this.transition.apply(fromSlide, toSlide, true);
    
    if (this.slides.getState(newIndex)) {
      this.restoreState(newIndex);
    } else {
      this.initNewSlide(newIndex);
    }
    
    this.slides.updateUI();
  },
  
  /**
   * Handle backward navigation
   */
  async backward() {
    if (this.isAnimating) return;
    this.isAnimating = true;
    
    try {
      const currentIndex = this.slides.currentIndex;
      const sequence = this.sequences.get(currentIndex);
      
      if (sequence?.hasPrevious()) {
        await this.animatePrevious(sequence);
        this.saveState(currentIndex);
      } else {
        const currentRepeat = this.repeatCount.get(currentIndex) || 1;
        
        if (currentRepeat > 1) {
          this.repeatCount.set(currentIndex, currentRepeat - 1);
          
          const elements = this.slides.getAnimationElements(currentIndex);
          sequence.reset();
          this.animation.makeElementsVisible(elements);
          
          const steps = sequence.animationSteps;
          for (let i = 0; i < steps.length; i++) {
            sequence.next();
          }
          
          this.saveState(currentIndex);
        } else {
          await this.moveToPreviousSlide();
        }
      }
    } finally {
      this.isAnimating = false;
    }
  },
  
  /**
   * Animate the previous element or animation
   * @param {AnimationSequence} sequence - Animation sequence
   */
  async animatePrevious(sequence) {
    const result = sequence.previous();
    if (!result) return;
    
    const { element, animation, isNewElement } = result;
    
    await this.animation.animateElement(element, animation, false);
    
    if (isNewElement) {
      const type = this.animation.getType(animation);
      if (type === 'show') element.style.opacity = '0';
      else if (type === 'hide') element.style.opacity = '1';
      this.animation.removeClasses(element);
    }
  },
  
  /**
   * Move to the previous slide
   */
  async moveToPreviousSlide() {
    const currentIndex = this.slides.currentIndex;
    this.saveState(currentIndex);
    
    if (!this.slides.previous()) return;
    
    const fromSlide = this.slides.get(currentIndex);
    const toSlide = this.slides.current();
    const newIndex = this.slides.currentIndex;
    
    await this.transition.apply(fromSlide, toSlide, false);
    
    if (this.slides.getState(newIndex)) {
      this.restoreState(newIndex);
    } else {
      const elements = this.slides.getAnimationElements(newIndex);
      this.animation.makeElementsVisible(elements);
    }
    
    this.slides.updateUI();
  },
  
  /**
   * Save the state of a slide
   * @param {number} index - Slide index
   */
  saveState(index) {
    const sequence = this.sequences.get(index);
    if (!sequence) return;
    
    const elements = sequence.getAll();
    const visibilityState = new Map();
    
    elements.forEach(element => {
      visibilityState.set(element, {
        isVisible: element.style.opacity === '1',
        animations: this.animation.parseAnimations(element)
      });
    });
    
    this.slides.saveState(index, {
      history: sequence.getHistory(),
      visibilityState,
      repeatCount: this.repeatCount.get(index) || 1
    });
  },
  
  /**
   * Restore the state of a slide
   * @param {number} index - Slide index
   */
  restoreState(index) {
    const state = this.slides.getState(index);
    if (!state) return;
    
    const sequence = this.sequences.get(index);
    if (!sequence) return;
    
    sequence.reset();
    const elements = sequence.getAll();
    this.animation.initElements(elements);
    
    if (state.visibilityState) {
      state.visibilityState.forEach((elementState, element) => {
        element.style.opacity = elementState.isVisible ? '1' : '0';
      });
    }
    
    sequence.restore(state.history);
    
    if (state.repeatCount) {
      this.repeatCount.set(index, state.repeatCount);
    }
  },
  
  /**
   * Initialize a new slide
   * @param {number} index - Slide index
   */
  initNewSlide(index) {
    const sequence = this.sequences.get(index);
    if (!sequence) return;
    
    const elements = sequence.getAll();
    this.animation.initElements(elements);

    this.repeatCount.set(index, 1);
  }
};

if (typeof PresentationCore !== 'undefined') {
  PresentationCore.register('controller', Controller);
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = Controller;
} 