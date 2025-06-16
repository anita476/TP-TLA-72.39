/**
 * Animation sequence module for the presentation framework
 */
class AnimationSequence {
  /**
   * Create a new animation sequence
   * @param {Array} elements - Elements to animate
   */
  constructor(elements = []) {
    this.elements = elements;
    this.currentIndex = 0;
    this.animatedElements = [];
    this.history = [];
  }
  
  /**
   * Check if there are more elements to animate
   * @returns {boolean} True if there are more elements
   */
  hasNext() {
    return this.currentIndex < this.elements.length;
  }
  
  /**
   * Check if there are previous elements to reverse
   * @returns {boolean} True if there are previous elements
   */
  hasPrevious() {
    return this.history.length > 0;
  }
  
  /**
   * Get the next element to animate
   * @returns {Element} Next element
   */
  next() {
    if (!this.hasNext()) return null;
    const element = this.elements[this.currentIndex];
    this.animatedElements.push(element);
    this.history.push({
      element,
      index: this.currentIndex,
      animation: element.dataset.animation
    });
    this.currentIndex++;
    return element;
  }
  
  /**
   * Get the previous element to reverse
   * @returns {Element} Previous element
   */
  previous() {
    if (!this.hasPrevious()) return null;
    const lastItem = this.history.pop();
    const element = lastItem.element;
    const index = this.animatedElements.lastIndexOf(element);
    if (index !== -1) this.animatedElements.splice(index, 1);
    this.currentIndex = lastItem.index;
    return element;
  }
  
  /**
   * Reset the sequence
   */
  reset() {
    this.currentIndex = 0;
    this.animatedElements = [];
    this.history = [];
  }
  
  /**
   * Get all elements
   * @returns {Array} All elements
   */
  getAll() {
    return this.elements;
  }
  
  /**
   * Get animated elements
   * @returns {Array} Animated elements
   */
  getAnimated() {
    return [...new Set(this.animatedElements)];
  }
  
  /**
   * Get non-animated elements
   * @returns {Array} Non-animated elements
   */
  getNonAnimated() {
    return this.elements.filter(el => !this.animatedElements.includes(el));
  }
  
  /**
   * Get animation history
   * @returns {Array} Animation history
   */
  getHistory() {
    return [...this.history];
  }
  
  /**
   * Restore from history
   * @param {Array} history - History to restore
   */
  restore(history) {
    if (!history?.length) return;
    this.reset();
    history.forEach(item => {
      const index = this.elements.indexOf(item.element);
      if (index !== -1) {
        this.currentIndex = index;
        this.animatedElements.push(item.element);
        this.history.push({ ...item });
        this.currentIndex++;
      }
    });
  }
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = AnimationSequence;
} 