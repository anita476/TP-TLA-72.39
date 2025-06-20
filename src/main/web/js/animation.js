const Animation = {
  animations: {
    'appear': { type: 'show', class: 'appear', reverseClass: 'disappear' },
    'disappear': { type: 'hide', class: 'disappear', reverseClass: 'appear' },
    'barrel-roll': { type: 'maintain', class: 'barrel-roll', reverseClass: 'barrel-roll-reverse' }
  },
  
  /**
   * Initialize the animation module
   * @param {Object} core - Core module reference
   */
  init(core) {
    this.core = core;
    this.config = core.get('config');
  },
  
  /**
   * Register a new animation
   * @param {string} name - Animation name
   * @param {Object} config - Animation configuration
   */
  register(name, config) {
    this.animations[name] = config;
    return this;
  },
  
  /**
   * Get animation configuration
   * @param {string} name - Animation name
   * @returns {Object} Animation configuration or null if not found
   */
  get(name) {
    return this.animations[name] || null;
  },
  
  /**
   * Get animation type (show, hide, maintain)
   * @param {string} name - Animation name
   * @returns {string} Animation type or null if not found
   */
  getType(name) {
    const animation = this.get(name);
    return animation ? animation.type : null;
  },
  
  /**
   * Get animation class based on direction
   * @param {string} name - Animation name
   * @param {boolean} isForward - Whether animation is forward
   * @returns {string} CSS class name or empty string if not found
   */
  getClass(name, isForward) {
    const animation = this.get(name);
    if (!animation) return '';
    if (isForward) return animation.class;
    return animation.reverseClass || animation.class;
  },
  
  /**
   * Initialize elements for animation
   * @param {Array} elements - Elements to initialize
   */
  initElements(elements) {
    if (!elements?.length) return;
    elements.forEach(element => {
      const animations = this.parseAnimations(element);
      if (!animations.length) {
        element.style.opacity = '1';
        return;
      }
      
      const animName = animations[0];
      const type = this.getType(animName);
      if (type === 'show') element.style.opacity = '0';
      else element.style.opacity = '1';
      this.removeClasses(element);
    });
  },
  
  /**
   * Make elements visible (without animation)
   * @param {Array} elements - Elements to make visible
   */
  makeElementsVisible(elements) {
    if (!elements?.length) return;
    elements.forEach(element => {
      const animations = this.parseAnimations(element);
      if (!animations.length) {
        element.style.opacity = '1';
        return;
      }
      
      const animName = animations[0];
      const type = this.getType(animName);
      if (type === 'hide') element.style.opacity = '0';
      else element.style.opacity = '1';
      this.removeClasses(element);
    });
  },
  
  /**
   * Remove all animation classes from an element
   * @param {Element} element - Element to clean
   */
  removeClasses(element) {
    const allClasses = Object.values(this.animations).flatMap(anim => 
      [anim.class, anim.reverseClass].filter(Boolean)
    );
    element.classList.remove(...allClasses);
  },
  
  /**
   * Parse animations from element
   * @param {Element} element - Element with data-animation attribute
   * @returns {Array} Array of animation names
   */
  parseAnimations(element) {
    const animAttr = element.dataset.animation || '';
    return animAttr.split(' ').filter(Boolean);
  },
  
  /**
   * Animate a single animation on an element
   * @param {Element} element - Element to animate
   * @param {string} animationName - Animation name
   * @param {boolean} isForward - Direction of animation
   * @returns {Promise} Promise that resolves when animation completes
   */
  animateElement(element, animationName, isForward) {
    return new Promise(resolve => {
      const animation = this.get(animationName);
      if (!animation) {
        element.style.opacity = '1';
        resolve();
        return;
      }
      
      const type = animation.type;
      if (type === 'show') element.style.opacity = isForward ? '0' : '1';
      else if (type === 'hide') element.style.opacity = isForward ? '1' : '0';
      
      const className = this.getClass(animationName, isForward);
      this.removeClasses(element);
      
      if (!className) {
        if (type === 'show') element.style.opacity = isForward ? '1' : '0';
        else if (type === 'hide') element.style.opacity = isForward ? '0' : '1';
        resolve();
        return;
      }
      
      setTimeout(() => {
        element.classList.add(className);
      }, 10);
      
      element.addEventListener('animationend', () => {
        if (type === 'show') element.style.opacity = isForward ? '1' : '0';
        else if (type === 'hide') element.style.opacity = isForward ? '0' : '1';
        resolve();
      }, { once: true });
    });
  }
};

if (typeof PresentationCore !== 'undefined') {
  PresentationCore.register('animation', Animation);
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = Animation;
} 