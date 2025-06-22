class AnimationSequence {
  /**
   * Create a new animation sequence
   * @param {Array} elements - Elements to animate
   */
  constructor(elements = []) {
    this.elements = elements.filter(el => {
      const animations = this.parseAnimations(el);
      return animations.length > 0;
    });
    
    this.animationSteps = [];
    this.elements.forEach(element => {
      const animations = this.parseAnimations(element);
      const animOrders = this.parseAnimationOrders(element, animations);
      
      animations.forEach((animation, index) => {
        this.animationSteps.push({
          element,
          animation,
          order: animOrders[index],
          animIndex: index
        });
      });
    });
    
    this.animationSteps.sort((a, b) => a.order - b.order);
    
    this.reset();
  }
  
  /**
   * Parse animation orders from element
   * @param {Element} element - Element with data-animation attribute
   * @param {Array} animations - Array of animation names
   * @returns {Array} Array of orders for each animation
   */
  parseAnimationOrders(element, animations) {
    if (element.dataset.animOrder) {
      if (element.dataset.animOrder.includes(',')) {
        const orders = element.dataset.animOrder.split(',').map(o => parseInt(o.trim(), 10));
        if (orders.length === animations.length) {
          return orders;
        }
      } else {
        const baseOrder = parseInt(element.dataset.animOrder, 10);
        return animations.map((_, index) => baseOrder + index);
      }
    }
    
    return animations.map((_, index) => 999 + index);
  }
  
  /**
   * Reset the sequence
   */
  reset() {
    this.currentStepIndex = 0;
    this.animatedElements = [];
    this.history = [];
    this.animationHistory = [];
  }
  
  /**
   * Parse animations from element
   * @param {Element} element - Element with data-animation attribute
   * @returns {Array} Array of animation names
   */
  parseAnimations(element) {
    const animAttr = element.dataset.animation || '';
    return animAttr.split(' ').filter(Boolean);
  }
  
  /**
   * Check if there are more steps to animate
   * @returns {boolean} True if there are more steps
   */
  hasNext() {
    return this.currentStepIndex < this.animationSteps.length;
  }
  
  /**
   * Check if there are previous steps to reverse
   * @returns {boolean} True if there are previous steps
   */
  hasPrevious() {
    return this.animationHistory.length > 0;
  }
  
  /**
   * Get the next animation step
   * @returns {Object} Object containing element and animation info
   */
  next() {
    if (!this.hasNext()) return null;
    
    const step = this.animationSteps[this.currentStepIndex];
    const { element, animation, animIndex } = step;
    
    const isNewElement = !this.animatedElements.includes(element);
    if (isNewElement) {
      this.animatedElements.push(element);
    }
    
    this.history.push({
      element,
      animation,
      animIndex,
      stepIndex: this.currentStepIndex,
      isNewElement
    });
    
    this.animationHistory.push({
      element,
      animation,
      isNewElement
    });
    
    this.currentStepIndex++;
    
    return { element, animation, isNewElement };
  }
  
  /**
   * Get the previous animation step to reverse
   * @returns {Object} Object containing element and animation info
   */
  previous() {
    if (!this.hasPrevious()) return null;
    
    const lastAnimation = this.animationHistory.pop();
    const lastHistoryItem = this.history.pop();
    
    if (lastHistoryItem.isNewElement) {
      const index = this.animatedElements.lastIndexOf(lastAnimation.element);
      if (index !== -1) this.animatedElements.splice(index, 1);
    }
    
    this.currentStepIndex = lastHistoryItem.stepIndex;
    
    return lastAnimation;
  }
  
  /**
   * Get all elements
   * @returns {Array} All elements
   */
  getAll() {
    return [...new Set(this.elements)];
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
      const { element, animation, stepIndex, isNewElement } = item;
      
      if (isNewElement && !this.animatedElements.includes(element)) {
        this.animatedElements.push(element);
      }
      
      this.history.push({ ...item });
      
      this.animationHistory.push({
        element,
        animation,
        isNewElement
      });
      
      if (stepIndex >= this.currentStepIndex) {
        this.currentStepIndex = stepIndex + 1;
      }
    });
  }
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = AnimationSequence;
}