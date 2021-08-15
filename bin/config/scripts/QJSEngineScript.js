//"use strict";

// noinspection ES6ConvertVarToLetConst
var MyScriptObject = {
	FunctionOne: function () {
		// noinspection JSUnresolvedVariable,JSUnresolvedFunction
		with (ExposedObject) {
			stdClog('Just some text passed to stdClog() method from exposed QObject.');
		}
	},

	FunctionTwo: function () {
		const Area = {
			rect: {x: 0, y: 0, w: 300, h: 200},
			rect_array: [0, 0, 300, 200],
			color: 'Green'
		};
		try {
			// noinspection JSUnresolvedVariable,JSUnresolvedFunction
			ExposedObject.passValue(Area);
		} catch (e) {
			ExposedObject.passValue(e);
		}
	}
};

// Report the object name to get callables from.
// noinspection BadExpressionStatementJS
"MyScriptObject";