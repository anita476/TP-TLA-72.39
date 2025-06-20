const UI = {
  elements: {},
  
  /**
   * Initialize the UI module
   * @param {Object} core - Core module reference
   */
  init(core) {
    this.core = core;
    this.elements = {
      slideNumber: document.querySelector('.slide-number')
    };
    this.slides = core.get('slides');
  },
  
  /**
   * Update the slide number indicator
   */
  updateSlideNumber() {
    if (!this.elements.slideNumber) return;
    if (!this.slides) this.slides = this.core.get('slides');
    if (!this.slides) return;
    const current = this.slides.currentIndex + 1;
    const total = this.slides.count();
    this.elements.slideNumber.textContent = `${current} / ${total}`;
  }
};

if (typeof PresentationCore !== 'undefined') {
  PresentationCore.register('ui', UI);
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = UI;
} 