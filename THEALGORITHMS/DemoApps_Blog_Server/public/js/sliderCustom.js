var slider = function() {
    var linkFun = function($scope, element, attrs) {
        var $slider = jQuery(element);
        var option = attrs;
        var tryPrseInt = function(key, option) {
            if (option[key]) {
                option[key] = parseInt(option[key]);
            }
        };

        tryPrseInt("min", option);
        tryPrseInt("max", option);
        tryPrseInt("step", option);

        option = jQuery.extend({
            value: $scope[option.ngModel],
            change: function(event, ui) {
                if (attrs.ngModel && ui.value != $scope[attrs.ngModel]) {
                    var express = attrs.ngModel + ' = ' + ui.value;
                    $scope.$apply(express);
                    if (attrs.ngChange) {
                        $scope.$eval(attrs.ngChange);
                    }
                }
            }
        }, option);
        $slider.slider(option);
        //back
        if (option.ngModel) {
            $scope.$watch(option.ngModel, function(val) {
                if (val != $slider.slider("value")) {
                    $slider.slider("value", val);
                }
            });
        }
        console.log(attrs);
    };
    return {
        restrict: 'E',
        replace: true,
        transclude: false,
        template: '<div />',
        link: linkFun
    };
};

myApp.directive("green-slider", slider);
//test controller
var test = function($scope) {
    $scope.value = 10;
    $scope.value2 = 10;
    $scope.change = function() {
        console.log("change", this.value);
    };
};



