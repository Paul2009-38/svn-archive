dojo.provide("cmyk.rules.Symbol");

dojo.require("cmyk.rules.renderingInstruction");

/**
	@lends cmyk.rules.Symbol
*/

dojo.declare("cmyk.rules.Symbol",cmyk.rules.renderingInstruction,{
	/** 
	      @constructs
	      @class This is a class representing a Symbol rendering instruction
	      @memberOf cmyk.rules
	      @extends cmyk.rules.renderingInstruction
	      @private
	*/
	constructor: function(node) {
//TODO: connect the real svg inside the object
		var _class="cmyk.rules.Symbol";

		var _attributeFactory = new cmyk.rules.attributes.attributeFactory();

		var _attributes = [];

		dojo.forEach(node.attributes, function(attribute,index,array) {
			_attributes.push(_attributeFactory.factory(attribute.nodeName,attribute.nodeValue,_class));
		});
	},
});
