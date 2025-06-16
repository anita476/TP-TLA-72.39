/**
 * Animation module for the presentation framework
 */
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
   * @returns {Object} Animation configuration
   */
  get(name) {
    return this.animations[name] || this.animations[this.config.defaultAnimation];
  },
  
  /**
   * Get animation type (show, hide, maintain)
   * @param {string} name - Animation name
   * @returns {string} Animation type
   */
  getType(name) {
    const animation = this.get(name);
    return animation ? animation.type : 'show';
  },
  
  /**
   * Get animation class based on direction
   * @param {string} name - Animation name
   * @param {boolean} isForward - Whether animation is forward
   * @returns {string} CSS class name
   */
  getClass(name, isForward) {
    const animation = this.get(name);
    if (!animation) return isForward ? 'appear' : 'disappear';
    if (isForward) return animation.class;
    return animation.reverseClass || animation.class;
  },
  
  /**
   * Animate an element
   * @param {Element} element - Element to animate
   * @param {boolean} isForward - Whether animation is forward
   * @returns {Promise} Promise that resolves when animation completes
   */
  animate(element, isForward) {
    return new Promise(resolve => {
      const animName = element.dataset.animation || this.config.defaultAnimation;
      const animation = this.get(animName);
      const type = animation.type;
      this.removeClasses(element);
      if (type === 'show') element.style.opacity = isForward ? '0' : '1';
      else if (type === 'hide') element.style.opacity = isForward ? '1' : '0';
      else element.style.opacity = '1';
      const className = this.getClass(animName, isForward);
      setTimeout(() => {
        element.classList.add(className);
      }, 10);
      element.addEventListener('animationend', () => {
        if (type === 'show') element.style.opacity = isForward ? '1' : '0';
        else if (type === 'hide') element.style.opacity = isForward ? '0' : '1';
        resolve();
      }, { once: true });
    });
  },
  
  /**
   * Initialize elements for animation
   * @param {Array} elements - Elements to initialize
   */
  initElements(elements) {
    if (!elements?.length) return;
    
    elements.forEach(element => {
      const animName = element.dataset.animation || this.config.defaultAnimation;
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
      const animName = element.dataset.animation || this.config.defaultAnimation;
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
  }
};

if (typeof PresentationCore !== 'undefined') {
  PresentationCore.register('animation', Animation);
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = Animation;
} 