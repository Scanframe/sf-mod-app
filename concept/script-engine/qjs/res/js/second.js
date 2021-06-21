"use strict";

// noinspection ES6ConvertVarToLetConst
var MyObject =
	{
		FunctionOne: function () {
			// noinspection JSUnresolvedVariable,JSUnresolvedFunction
			ExposedObject.stdClog('Just some text passed to stdClog() method from exposed QObject.');
		},

		FunctionTwo: function () {
			const Area = {
				rect: {x: 0, y: 0, w: 300, h: 200},
				rect_array: [0, 0, 300, 200],
				color: 'Green'
			};
			// noinspection JSUnresolvedVariable,JSUnresolvedFunction
			ExposedObject.passValue(Area);
		}
	};

// Report the object name to get callables from.
// noinspection BadExpressionStatementJS
"MyObject";