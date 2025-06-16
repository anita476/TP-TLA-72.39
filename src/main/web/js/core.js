/**
 * Core module system for the presentation framework
 */
const PresentationCore = {
  modules: {},
  
  /**
   * Register a module
   * @param {string} name - Module name
   * @param {Object} module - Module implementation
   */
  register(name, module) {
    this.modules[name] = module;
    return module;
  },
  
  /**
   * Get a registered module
   * @param {string} name - Module name
   * @returns {Object} The requested module
   */
  get(name) {
    return this.modules[name];
  },
  
  /**
   * Initialize all registered modules
   * @param {Object} config - Configuration object
   */
  init(config = {}) {
    const initOrder = ['config', 'animation', 'transition', 'slides', 'controller', 'ui'];
    if (!this.modules.config) {
      this.register('config', {
        ...config,
        defaultTransition: 'fade',
        defaultAnimation: 'appear'
      });
    }
    initOrder.forEach(moduleName => {
      const module = this.modules[moduleName];
      if (module && typeof module.init === 'function') {
        module.init(this);
      }
    });
    return this;
  }
};

if (typeof module !== 'undefined' && module.exports) {
  module.exports = PresentationCore;
} 