/**
 * Slides module for the presentation framework
 */
const Slides = {
  slides: [],
  currentIndex: 0,
  history: [],
  slideStates: new Map(),
  
  /**
   * Initialize the slides module
   * @param {Object} core - Core module reference
   */
  init(core) {
    this.core = core;
    this.config = core.get('config');
    this.animation = core.get('animation');
    this.slides = Array.from(document.querySelectorAll('.slide'));
    this.slides.forEach(slide => {
      const elements = Array.from(slide.querySelectorAll('[data-animation]'));
      this.animation.initElements(elements);
    });
    if (this.slides.length > 0) {
      this.history = [0];
      this.showSlide(0);
    }
    setTimeout(() => this.updateUI(), 100);
  },
  
  /**
   * Get the current slide
   * @returns {Element} Current slide element
   */
  current() {
    return this.slides[this.currentIndex];
  },
  
  /**
   * Get a slide by index
   * @param {number} index - Slide index
   * @returns {Element} Slide element
   */
  get(index) {
    return this.slides[index];
  },
  
  /**
   * Show a specific slide
   * @param {number} index - Slide index to show
   */
  showSlide(index) {
    if (index < 0 || index >= this.slides.length) return;
    this.slides.forEach(slide => slide.classList.remove('active'));
    this.slides[index].classList.add('active');
    this.currentIndex = index;
    this.updateUI();
  },
  
  /**
   * Update UI elements like slide number
   */
  updateUI() {
    const ui = this.core.get('ui');
    if (ui && typeof ui.updateSlideNumber === 'function') {
      ui.updateSlideNumber();
    }
  },
  
  /**
   * Move to the next slide
   * @returns {boolean} True if successful
   */
  next() {
    if (this.currentIndex >= this.slides.length - 1) return false;
    this.history.push(this.currentIndex + 1);
    this.currentIndex++;
    this.updateUI();
    return true;
  },
  
  /**
   * Move to the previous slide
   * @returns {boolean} True if successful
   */
  previous() {
    if (this.history.length <= 1) return false;
    this.history.pop();
    this.currentIndex = this.history[this.history.length - 1];
    this.updateUI();
    return true;
  },
  
  /**
   * Get animation elements for the current slide
   * @returns {Array} Animation elements
   */
  getAnimationElements(index = this.currentIndex) {
    const slide = this.slides[index];
    return slide ? Array.from(slide.querySelectorAll('[data-animation]')) : [];
  },
  
  /**
   * Save the current state of a slide
   * @param {number} index - Slide index
   * @param {Object} state - State to save
   */
  saveState(index, state) {
    this.slideStates.set(index, state);
  },
  
  /**
   * Get the saved state of a slide
   * @param {number} index - Slide index
   * @returns {Object} Saved state
   */
  getState(index) {
    return this.slideStates.get(index);
  },
  
  /**
   * Check if the current slide is the first slide
   * @returns {boolean} True if on first slide
   */
  isFirst() {
    return this.currentIndex === 0;
  },
  
  /**
   * Check if the current slide is the last slide
   * @returns {boolean} True if on last slide
   */
  isLast() {
    return this.currentIndex === this.slides.length - 1;
  },
  
  /**
   * Get the total number of slides
   * @returns {number} Total slides
   */
  count() {
    return this.slides.length;
  }
};

if (typeof PresentationCore !== 'undefined') {
  PresentationCore.register('slides', Slides);
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = Slides;
} 