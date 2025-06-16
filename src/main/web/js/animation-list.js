/**
 * AnimationList class
 * Manages a list of elements to be animated in sequence
 */
class AnimationList {
    /**
     * Create a new AnimationList
     * @param {Array} elements - Array of DOM elements to animate
     */
    constructor(elements) {
        this.elements = elements || [];
        this.reset();
    }

    /**
     * Reset the list to its initial state
     */
    reset() {
        this.currentIndex = 0;
        this.animatedElements = [];
        this.animationSequence = [];
    }

    /**
     * Check if there are more elements to animate
     * @returns {boolean} True if there are more elements
     */
    hasNext() {
        return this.currentIndex < this.elements.length;
    }

    /**
     * Check if there are previous elements that can be reversed
     * @returns {boolean} True if there are previous elements
     */
    hasPrevious() {
        return this.animationSequence.length > 0;
    }

    /**
     * Get the next element to animate
     * @returns {Element|null} The next element or null if none
     */
    next() {
        if (!this.hasNext()) return null;
        const element = this.elements[this.currentIndex];
        const animation = element.dataset.animation || 'appear';
        this.animatedElements.push(element);
        this.animationSequence.push({
            element,
            index: this.currentIndex,
            animation
        });
        this.currentIndex++;
        return element;
    }

    /**
     * Get the previous element that was animated to reverse its animation
     * @returns {Element|null} The previous element or null if none
     */
    previous() {
        if (!this.hasPrevious()) return null;
        const lastAnimation = this.animationSequence.pop();
        const element = lastAnimation.element;
        const lastIndex = this.animatedElements.lastIndexOf(element);
        if (lastIndex !== -1) this.animatedElements.splice(lastIndex, 1);
        this.currentIndex = lastAnimation.index;
        return element;
    }

    /**
     * Get all elements in the list
     * @returns {Array} All elements
     */
    getAllElements() {
        return this.elements;
    }

    /**
     * Get all currently animated elements
     * @returns {Array} All animated elements
     */
    getAnimatedElements() {
        return [...new Set(this.animatedElements)];
    }

    /**
     * Get all elements that haven't been animated yet
     * @returns {Array} All non-animated elements
     */
    getNonAnimatedElements() {
        return this.elements.filter(element => !this.animatedElements.includes(element));
    }
    
    /**
     * Get the complete animation history
     * @returns {Array} The animation sequence
     */
    getAnimationSequence() {
        return this.animationSequence;
    }
    
    /**
     * Restore a specific animation state
     * @param {Array} animationSequence - The animation sequence to restore
     */
    restoreAnimationState(animationSequence) {
        if (!animationSequence?.length) return;
        this.reset();
        animationSequence.forEach(animation => {
            const index = this.elements.indexOf(animation.element);
            if (index !== -1) {
                this.currentIndex = index;
                this.animatedElements.push(animation.element);
                this.animationSequence.push({ ...animation });
                this.currentIndex++;
            }
        });
    }
} 