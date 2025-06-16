/**
 * Controller module for the presentation framework
 */
const Controller = {
  isAnimating: false,
  sequences: new Map(),
  
  /**
   * Initialize the controller
   * @param {Object} core - Core module reference
   */
  init(core) {
    this.core = core;
    this.slides = core.get('slides');
    this.animation = core.get('animation');
    this.transition = core.get('transition');
    this.slides.slides.forEach((slide, index) => {
      const elements = this.slides.getAnimationElements(index);
      this.sequences.set(index, new AnimationSequence(elements));
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
        await this.moveToNextSlide();
      }
    } finally {
      this.isAnimating = false;
    }
  },
  
  /**
   * Animate the next element
   * @param {AnimationSequence} sequence - Animation sequence
   */
  async animateNext(sequence) {
    const element = sequence.next();
    if (!element) return;
    await this.animation.animate(element, true);
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
        await this.moveToPreviousSlide();
      }
    } finally {
      this.isAnimating = false;
    }
  },
  
  /**
   * Animate the previous element
   * @param {AnimationSequence} sequence - Animation sequence
   */
  async animatePrevious(sequence) {
    const element = sequence.previous();
    if (!element) return;
    this.animation.removeClasses(element);
    await new Promise(resolve => setTimeout(resolve, 10));
    await this.animation.animate(element, false);
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
        animation: element.dataset.animation
      });
    });
    this.slides.saveState(index, {
      history: sequence.getHistory(),
      visibilityState
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
  }
};

if (typeof PresentationCore !== 'undefined') {
  PresentationCore.register('controller', Controller);
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = Controller;
} 