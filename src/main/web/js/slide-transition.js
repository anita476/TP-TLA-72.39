/**
 * SlideTransition class
 * Handles transitions between slides with configurable effects
 */
class SlideTransition {
    static TRANSITION_REGISTRY = {
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
            duration: 800,
            customEnter: (element, isForward) => {
                if (!isForward) {
                    element.style.transform = 'scale(1.1)';
                    element.style.opacity = '0';
                    void element.offsetWidth;
                    element.style.transition = 'transform 0.8s, opacity 0.8s';
                    element.style.transform = 'scale(1)';
                    element.style.opacity = '1';
                }
            }
        }
    };

    /**
     * Register a new transition type
     * @param {string} name - Name of the transition
     * @param {Object} config - Transition configuration
     * @param {string|Function} config.enter - Enter class or function returning class
     * @param {string|Function} config.exit - Exit class or function returning class
     * @param {number} config.duration - Transition duration in ms
     * @param {Function} [config.customEnter] - Custom enter animation function
     * @param {Function} [config.customExit] - Custom exit animation function
     */
    static registerTransition(name, config) {
        this.TRANSITION_REGISTRY[name] = config;
    }

    /**
     * Remove all transition classes from an element
     * @param {Element} element - Element to clean
     */
    static removeTransitionClasses(element) {
        const allClasses = Object.values(this.TRANSITION_REGISTRY).flatMap(config => {
            const classes = [];
            if (typeof config.enter === 'string') classes.push(config.enter);
            if (typeof config.exit === 'string') classes.push(config.exit);
            return classes;
        });
        element.classList.remove('slide-transition', ...allClasses);
        element.style.transform = '';
        element.style.transition = '';
        element.style.opacity = '';
    }

    /**
     * Apply a transition between slides
     * @param {Element} currentSlide - The current slide
     * @param {Element} nextSlide - The next slide
     * @param {boolean} isForward - Whether the transition is forward or backward
     * @returns {Promise} A promise that resolves when the transition is complete
     */
    static async transition(currentSlide, nextSlide, isForward) {
        const transitionType = isForward ? 
            nextSlide.dataset.transition || 'fade' : 
            currentSlide.dataset.transition || 'fade';

        const transitionConfig = this.TRANSITION_REGISTRY[transitionType] || this.TRANSITION_REGISTRY.fade;
        
        this.removeTransitionClasses(currentSlide);
        this.removeTransitionClasses(nextSlide);
        
        currentSlide.classList.add('slide-transition');
        nextSlide.classList.add('slide-transition');
        
        this.applyTransition(currentSlide, nextSlide, transitionConfig, isForward);
        
        nextSlide.style.zIndex = '1';
        currentSlide.style.zIndex = '2';
        nextSlide.classList.add('active');
        
        return new Promise(resolve => {
            setTimeout(() => {
                currentSlide.classList.remove('active');
                currentSlide.style.zIndex = '';
                nextSlide.style.zIndex = '';
                this.removeTransitionClasses(currentSlide);
                this.removeTransitionClasses(nextSlide);
                resolve();
            }, transitionConfig.duration);
        });
    }
    
    /**
     * Apply transition based on configuration
     * @param {Element} currentSlide - The current slide
     * @param {Element} nextSlide - The next slide
     * @param {Object} config - Transition configuration
     * @param {boolean} isForward - Whether the transition is forward or backward
     */
    static applyTransition(currentSlide, nextSlide, config, isForward) {
        if (typeof config.exit === 'function') {
            currentSlide.classList.add(config.exit(isForward));
        } else {
            currentSlide.classList.add(config.exit);
        }

        if (typeof config.enter === 'function') {
            nextSlide.classList.add(config.enter(isForward));
        } else {
            nextSlide.classList.add(config.enter);
        }

        if (config.customEnter) {
            config.customEnter(nextSlide, isForward);
        }
        if (config.customExit) {
            config.customExit(currentSlide, isForward);
        }
    }
} 