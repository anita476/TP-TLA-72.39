/**
 * Transition module for the presentation framework
 */
const Transition = {
  transitions: {
    fade: {
      enter: 'fade-in',
      exit: 'fade-out',
      duration: 800
    },
    slide: {
      enter: (isForward) => isForward ? 'slide-in-right' : 'slide-in-left',
      exit: (isForward) => isForward ? 'slide-out-left' : 'slide-out-right',
      duration: 800
    },
    jump: {
      enter: 'jump-in',
      exit: 'fade-out',
      duration: 800
    }
  },
  
  /**
   * Initialize the transition module
   * @param {Object} core - Core module reference
   */
  init(core) {
    this.core = core;
    this.config = core.get('config');
  },
  
  /**
   * Register a new transition
   * @param {string} name - Transition name
   * @param {Object} config - Transition configuration
   */
  register(name, config) {
    this.transitions[name] = config;
    return this;
  },
  
  /**
   * Get transition configuration
   * @param {string} name - Transition name
   * @returns {Object} Transition configuration
   */
  get(name) {
    return this.transitions[name] || this.transitions[this.config.defaultTransition];
  },
  
  /**
   * Apply a transition between slides
   * @param {Element} from - The current slide
   * @param {Element} to - The next slide
   * @param {boolean} isForward - Whether transition is forward
   * @returns {Promise} Promise that resolves when transition completes
   */
  apply(from, to, isForward) {
    const transitionName = to.dataset.transition || this.config.defaultTransition;
    const config = this.get(transitionName);
    this.removeClasses(from);
    this.removeClasses(to);
    from.classList.add('slide-transition');
    to.classList.add('slide-transition');
    const exitClass = typeof config.exit === 'function' ? config.exit(isForward) : config.exit;
    const enterClass = typeof config.enter === 'function' ? config.enter(isForward) : config.enter;
    from.classList.add(exitClass);
    to.classList.add(enterClass);
    if (config.customExit) config.customExit(from, isForward);
    if (config.customEnter) config.customEnter(to, isForward);
    to.style.zIndex = '1';
    from.style.zIndex = '2';
    to.classList.add('active');
    return new Promise(resolve => {
      setTimeout(() => {
        from.classList.remove('active');
        from.style.zIndex = '';
        to.style.zIndex = '';
        this.removeClasses(from);
        this.removeClasses(to);
        resolve();
      }, config.duration);
    });
  },
  
  /**
   * Remove all transition classes from an element
   * @param {Element} element - Element to clean
   */
  removeClasses(element) {
    const allClasses = Object.values(this.transitions).flatMap(config => {
      const classes = [];
      if (typeof config.enter === 'string') classes.push(config.enter);
      if (typeof config.exit === 'string') classes.push(config.exit);
      return classes;
    });    
    element.classList.remove('slide-transition', ...allClasses);
    element.style.transform = '';
    element.style.transition = '';
  }
};

if (typeof PresentationCore !== 'undefined') {
  PresentationCore.register('transition', Transition);
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = Transition;
} 