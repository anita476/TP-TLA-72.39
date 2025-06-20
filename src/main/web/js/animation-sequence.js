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
    this.reset();
  }
  
  /**
   * Reset the sequence
   */
  reset() {
    this.currentIndex = 0;
    this.animatedElements = [];
    this.history = [];
    this.currentAnimationIndex = new Map();
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
   * Check if there are more elements to animate
   * @returns {boolean} True if there are more elements
   */
  hasNext() {
    if (this.currentIndex < this.elements.length) return true;
    
    if (this.history.length > 0) {
      const lastItem = this.history[this.history.length - 1];
      const element = lastItem.element;
      const animations = this.parseAnimations(element);
      const currentAnimIndex = this.currentAnimationIndex.get(element) || 0;
      return currentAnimIndex < animations.length - 1;
    }
    
    return false;
  }
  
  /**
   * Check if there are previous elements to reverse
   * @returns {boolean} True if there are previous elements or animations
   */
  hasPrevious() {
    return this.animationHistory.length > 0;
  }
  
  /**
   * Get the next element or animation to animate
   * @returns {Object} Object containing element and animation info
   */
  next() {
    if (!this.hasNext()) return null;
    
    if (this.history.length > 0) {
      const lastItem = this.history[this.history.length - 1];
      const element = lastItem.element;
      const animations = this.parseAnimations(element);
      let currentAnimIndex = this.currentAnimationIndex.get(element) || 0;
      
      if (currentAnimIndex < animations.length - 1) {
        currentAnimIndex++;
        this.currentAnimationIndex.set(element, currentAnimIndex);
        const animation = animations[currentAnimIndex];
        
        lastItem.currentAnimation = animation;
        lastItem.animationIndex = currentAnimIndex;
        
        this.animationHistory.push({
          element,
          animation,
          isNewElement: false
        });
        
        return { element, animation, isNewElement: false };
      }
    }
    
    const element = this.elements[this.currentIndex];
    if (!element) return null;
    
    this.animatedElements.push(element);
    
    const animations = this.parseAnimations(element);
    if (!animations.length) {
      this.currentIndex++;
      return this.next();
    }
    
    const animation = animations[0];
    
    this.currentAnimationIndex.set(element, 0);
    this.history.push({
      element,
      index: this.currentIndex,
      animations,
      currentAnimation: animation,
      animationIndex: 0
    });
    
    this.animationHistory.push({
      element,
      animation,
      isNewElement: true
    });
    
    this.currentIndex++;
    
    return { element, animation, isNewElement: true };
  }
  
  /**
   * Get the previous element or animation to reverse
   * @returns {Object} Object containing element and animation info
   */
  previous() {
    if (!this.hasPrevious()) return null;
    
    const lastAnimation = this.animationHistory.pop();
    
    if (lastAnimation.isNewElement) {
      const lastItem = this.history.pop();
      
      const index = this.animatedElements.lastIndexOf(lastAnimation.element);
      if (index !== -1) this.animatedElements.splice(index, 1);
      
      this.currentIndex = lastItem.index;
      
      if (this.history.length > 0) {
        const prevItem = this.history[this.history.length - 1];
        this.currentAnimationIndex.set(prevItem.element, prevItem.animationIndex);
      }
    } else {
      const lastItem = this.history[this.history.length - 1];
      const element = lastItem.element;
      const currentAnimIndex = this.currentAnimationIndex.get(element);
      
      this.currentAnimationIndex.set(element, currentAnimIndex - 1);
      lastItem.currentAnimation = lastItem.animations[currentAnimIndex - 1];
      lastItem.animationIndex = currentAnimIndex - 1;
    }
    
    return lastAnimation;
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
        
        if (item.animationIndex !== undefined) {
          this.currentAnimationIndex.set(item.element, item.animationIndex);
        }
        
        this.currentIndex++;
      }
    });
    
    this.history.forEach(item => {
      const element = item.element;
      const animations = item.animations;
      const currentAnimIndex = item.animationIndex || 0;
      
      for (let i = 0; i <= currentAnimIndex; i++) {
        this.animationHistory.push({
          element,
          animation: animations[i],
          isNewElement: i === 0
        });
      }
    });
  }
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = AnimationSequence;
}