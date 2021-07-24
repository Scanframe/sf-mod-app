"use strict";

// noinspection ES6ConvertVarToLetConst
var Main = {};

// First from C++ callable function/method.
Main.First = function () {
	// noinspection JSUnresolvedVariable,JSUnresolvedFunction
	ExposedObject.stdClog('Just some text passed to stdClog() method from exposed QObject.');
}

// Second from C++ callable function/method.
Main.Second = function () {
	// noinspection JSUnresolvedVariable,JSUnresolvedFunction
	const myCar = {
		make: 'Ford',
		model: 'Mustang',
		year: 1969,
		price: 123.456
	};
	ExposedObject.passValue(myCar);
}

// First from C++ callable function/method.
Main.Third = function () {
	// noinspection JSUnresolvedVariable,JSUnresolvedFunction
	ExposedObject.passValue('Just a string passed to passValue().');
}

console.info('Hello World info!');
console.log('Hello World log!');
console.error('Hello World error!');
// Report the object name to get callables from.
// noinspection BadExpressionStatementJS
"Main";