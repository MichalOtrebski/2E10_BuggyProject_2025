#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* html = R"rawliteral(

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="stylesheet" href="style.css">
  <title>Interactive Dashboard</title>
</head>
<body>
  <div class="container">
    <!-- Left Panel -->
    <div class="left-panel">
      <div class="controls">
        <div class="buttons">
          <button id="startStopBtn" class="start">Start</button>
          <button id="changeMode" class="changemode">Reference Speed</button>
        </div>
        <div class="top-right mobile-only" style="height: 150px;">
          <img src="" class="images">
        </div>
        <div class="speedometer-section">
            <canvas data-type="radial-gauge"
                data-width="300"
                data-height="300"
                data-units="cm/s"
                data-title="Speed"
                data-value="0"
                data-min-value="0"
                data-max-value="50"
                data-major-ticks="0,10,20,30,40,50"
                data-minor-ticks="4"
                data-stroke-ticks="true"
                data-highlights='[
                    { "from": 0, "to": 35, "color": "rgba(0,70,20,140)" },
                    { "from": 35, "to": 50, "color": "rgba(150,0,0,140)" }
                ]'
                data-color-plate="#222"
                data-color-major-ticks="#f5f5f5"
                data-color-minor-ticks="#ddd"
                data-color-title="#fff"
                data-color-units="#ccc"
                data-color-numbers="#eee"
                data-color-needle-start="rgba(240, 128, 128, 1)"
                data-color-needle-end="rgba(255, 160, 122, .9)"
                data-value-box="true"
                data-animation-rule="quint"
                data-animation-duration="200"
                data-font-value="Led"
                data-animated-value="true"
            ></canvas>
            <div class="slidecontainer">
                <input type="range" step="0.1" max="50" value="50" class="slider" id="speedslider">
                <div style="margin-top: 5px; align-self: center;" id="speedSet">Speed: 0 cm/s</div>
            </div>
        </div>
        <!-- Distance and Travelled side by side -->
        <div class="distance-container">
          <div class="distance-section">
            <h3>Distance</h3>
            <p>0 cm</p>
          </div>
          <div class="distance-section">
            <h3>Travelled</h3>
            <p>0 cm</p>
          </div>
        </div>
      </div>
      <div class="debug-panel">
        <h3>Debug Panel</h3>
        <div class="variables">
          <p>Variable 1: <span>Value</span></p>
          <p>Variable 2: <span>Value</span></p>
          <p>Variable 3: <span>Value</span></p>
          <p>Variable 4: <span>Value</span></p>
        </div>
      </div>
    </div>
    
    <!-- Right Panel (split into top and bottom sections) -->
    <div class="right-panel">
      <div class="top-right">
        <img src="" class="images">
      </div>
      <div class="bottom-right">
        <canvas class="canvas-container" id="odometry" width="750" height="400" style="border:2px solid #444;"></canvas>
      </div>
    </div>
  </div>
  <script>
    !function(e){"use strict";function t(e,t){if(!e)throw new ReferenceError("this hasn't been initialised - super() hasn't been called");return!t||"object"!=typeof t&&"function"!=typeof t?e:t}function i(e,t){if("function"!=typeof t&&null!==t)throw new TypeError("Super expression must either be null or a function, not "+typeof t);e.prototype=Object.create(t&&t.prototype,{constructor:{value:e,enumerable:!1,writable:!0,configurable:!0}}),t&&(Object.setPrototypeOf?Object.setPrototypeOf(e,t):e.__proto__=t)}function r(e,t){if(!(e instanceof t))throw new TypeError("Cannot call a class as a function")}function n(e,t){if(t||(t="undefined"==typeof window?global:window),void 0!==t[e])return t[e];for(var i=["webkit","moz","ms","o"],r=0,n=i.length,o=e.charAt(0).toUpperCase()+e.substr(1);r<n;r++){var a=t[i[r]+o];if(void 0!==a)return a}return null}function o(e,t,i,r,n,a,l){if("function"!=typeof r)throw new TypeError("Invalid animation rule:",r);var s=e-i,d=s/n,u=0;d>1&&(d=1),1!==d&&(u=r(d),isFinite(u)&&!isNaN(u)&&(d=u)),t&&t(d),s<n?l.frame=U(function(e){return o(e,t,i,r,n,a,l)}):(a&&a(),l.inProgress=!1)}function a(){Array.prototype.constructor.apply(this,arguments)}function l(e){if(!(e instanceof DOMException&&2152923147===e.result))throw e}function s(e,t){return t.replace(ee,function(t,i){var r=e[i];return void 0!==r?r:t})}function d(e){return e.majorTicks instanceof Array||(e.majorTicks=e.majorTicks?[e.majorTicks]:[]),e.majorTicks.length||(e.majorTicks.push(te.formatMajorTickNumber(e.minValue,e)),e.majorTicks.push(te.formatMajorTickNumber(e.maxValue,e))),["right"!==e.tickSide,"left"!==e.tickSide]}function u(e,t,i,r,n,o){e.beginPath(),e.moveTo(t+o,i),e.lineTo(t+r-o,i),e.quadraticCurveTo(t+r,i,t+r,i+o),e.lineTo(t+r,i+n-o),e.quadraticCurveTo(t+r,i+n,t+r-o,i+n),e.lineTo(t+o,i+n),e.quadraticCurveTo(t,i+n,t,i+n-o),e.lineTo(t,i+o),e.quadraticCurveTo(t,i,t+o,i),e.closePath()}function c(e,t){var i=t.valueDec,r=t.valueInt,n=0,o=void 0,a=void 0,l=void 0;if(e=parseFloat(e),l=e<0,e=Math.abs(e),i>0){for(a=e.toFixed(i).toString().split("."),o=r-a[0].length;n<o;++n)a[0]="0"+a[0];a=(l?"-":"")+a[0]+"."+a[1]}else{for(a=Math.round(e).toString(),o=r-a.length;n<o;++n)a="0"+a;a=(l?"-":"")+a}return a}function h(e,t){var i=void 0,r=!1;return i=0===t.majorTicksDec?Math.round(e).toString():e.toFixed(t.majorTicksDec),t.majorTicksInt>1?(r=~i.indexOf("."),~i.indexOf("-")?"-"+[t.majorTicksInt+t.majorTicksDec+2+(r?1:0)-i.length].join("0")+i.replace("-",""):[t.majorTicksInt+t.majorTicksDec+1+(r?1:0)-i.length].join("0")+i):i}function f(e){return e*Math.PI/180}function v(e,t){return{x:-e*Math.sin(t),y:e*Math.cos(t)}}function m(e,t,i,r){var n=!(arguments.length>4&&void 0!==arguments[4])||arguments[4],o=arguments.length>5&&void 0!==arguments[5]?arguments[5]:0,a=e.createLinearGradient(n?0:o,n?o:0,n?0:r,n?r:0);return a.addColorStop(0,t),a.addColorStop(1,i),a}function g(e,t){if(arguments.length>2&&void 0!==arguments[2]&&arguments[2])return e.restore(),!0;e.save();var i=t.borderShadowWidth;return i&&(e.shadowBlur=i,e.shadowColor=t.colorBorderShadow),!0}function p(e,t){t.needleShadow&&(e.shadowOffsetX=2,e.shadowOffsetY=2,e.shadowBlur=10,e.shadowColor=t.colorNeedleShadowDown)}function w(e,t,i){return e["font"+t+"Style"]+" "+e["font"+t+"Weight"]+" "+e["font"+t+"Size"]*i+"px "+e["font"+t]}function b(e){e.shadowOffsetX=null,e.shadowOffsetY=null,e.shadowBlur=null,e.shadowColor="",e.strokeStyle=null,e.lineWidth=0,e.save()}function y(e,t,i,r){t.valueTextShadow&&(e.shadowOffsetX=i,e.shadowOffsetY=i,e.shadowBlur=r,e.shadowColor=t.colorValueTextShadow)}function x(e,t,i,r,n,o){if(t.valueBox){b(e);var a=t.valueDec?1+t.valueDec:0,l="9".repeat(Math.max.apply(null,[String(parseInt(i)).length+a].concat(t.majorTicks.map(function(e){return String(parseInt(e,10)).length+a})))),s=t.valueText||c(i,t),d=o/200,h=o/100,f=.4*h,v=1.2*h;e.font=w(t,"Value",d),y(e,t,f,v);var m=e.measureText(t.valueText?s:"-"+c(Number(l),t)).width;b(e);var g=parseFloat(t.fontValueSize)*d+f+v,p=h*parseFloat(t.valueBoxStroke),x=2*o-2*p,k=m+10*h,T=1.1*g+f+v,S=h*t.valueBoxBorderRadius,O=(parseFloat(t.valueBoxWidth)||0)/100*x;O>k&&(k=O),k>x&&(k=x);var A=r-k/2,V=n-T/2,C=n-5.75*h;if(e.beginPath(),S?u(e,A,V,k,T,S):e.rect(A,V,k,T),p){var P=e.createRadialGradient(r,C,10*h,r,C,20*h);P.addColorStop(0,t.colorValueBoxRect),P.addColorStop(1,t.colorValueBoxRectEnd),e.strokeStyle=P,e.lineWidth=p,e.stroke()}t.colorValueBoxShadow&&(e.shadowBlur=1.2*h,e.shadowColor=t.colorValueBoxShadow),t.colorValueBoxBackground&&(e.fillStyle=t.colorValueBoxBackground,e.fill()),e.closePath(),e.restore(),y(e,t,f,v),e.fillStyle=t.colorValueText,e.textAlign="center",e.textBaseline="alphabetic",e.fillText(s,A+k/2,n+T/2-g/3),e.restore()}}function k(e){var t=e.value,i=e.minValue,r=e.maxValue,n=.01*(r-i);return{normal:t<i?i:t>r?r:t,indented:t<i?i-n:t>r?r+n:t}}function T(e,t,i,r,n){i.beginPath(),i.arc(0,0,$(e),0,2*ie,!0),i.lineWidth=t,i.strokeStyle=n?te.linearGradient(i,r,n,e):r,i.stroke(),i.closePath()}function S(e,t){var i=H.pixelRatio;return e.maxRadius||(e.maxRadius=e.max-t.borderShadowWidth-t.borderOuterWidth*i-t.borderMiddleWidth*i-t.borderInnerWidth*i+(t.borderOuterWidth?.5:0)+(t.borderMiddleWidth?.5:0)+(t.borderInnerWidth?.5:0)),e.maxRadius}function O(e,t){var i=H.pixelRatio,r=t.borderShadowWidth*i,n=e.max-r-t.borderOuterWidth*i/2,o=n-t.borderOuterWidth*i/2-t.borderMiddleWidth*i/2+.5,a=o-t.borderMiddleWidth*i/2-t.borderInnerWidth*i/2+.5,l=S(e,t),s=void 0,d=!1;e.save(),t.borderOuterWidth&&(d=te.drawShadow(e,t,d),T(n,t.borderOuterWidth*i,e,t.colorBorderOuter,t.colorBorderOuterEnd)),t.borderMiddleWidth&&(d=te.drawShadow(e,t,d),T(o,t.borderMiddleWidth*i,e,t.colorBorderMiddle,t.colorBorderMiddleEnd)),t.borderInnerWidth&&(d=te.drawShadow(e,t,d),T(a,t.borderInnerWidth*i,e,t.colorBorderInner,t.colorBorderInnerEnd)),te.drawShadow(e,t,d),e.beginPath(),e.arc(0,0,$(l),0,2*ie,!0),t.colorPlateEnd?(s=e.createRadialGradient(0,0,l/2,0,0,l),s.addColorStop(0,t.colorPlate),s.addColorStop(1,t.colorPlateEnd)):s=t.colorPlate,e.fillStyle=s,e.fill(),e.closePath(),e.restore()}function A(e,t){var i=e.max*(parseFloat(t.highlightsWidth)||0)/100;if(i){var r=$(C(e,t)-i/2),n=0,o=t.highlights.length,a=(t.maxValue-t.minValue)/t.ticksAngle;for(e.save();n<o;n++){var l=t.highlights[n];e.beginPath(),e.rotate(re),e.arc(0,0,r,te.radians(t.startAngle+(l.from-t.minValue)/a),te.radians(t.startAngle+(l.to-t.minValue)/a),!1),e.strokeStyle=l.color,e.lineWidth=i,e.lineCap=t.highlightsLineCap,e.stroke(),e.closePath(),e.restore(),e.save()}}}function V(e,t){var i=C(e,t),r=void 0,n=void 0,o=void 0,a=0,l=0,s=Math.abs(t.minorTicks)||0,d=t.ticksAngle/(t.maxValue-t.minValue);for(e.lineWidth=H.pixelRatio,e.strokeStyle=t.colorMinorTicks||t.colorStrokeTicks,e.save(),t.exactTicks?(n=t.maxValue-t.minValue,r=s?n/s:0,l=(Q.mod(t.majorTicks[0],s)||0)*d):r=s*(t.majorTicks.length-1);a<r;++a)(o=t.startAngle+l+a*(t.ticksAngle/r))<=t.ticksAngle+t.startAngle&&(e.rotate(te.radians(o)),e.beginPath(),e.moveTo(0,i),e.lineTo(0,i-.075*e.max),j(e))}function C(e,t){var i=e.max/100;return S(e,t)-5*i-(t.barWidth?2*(parseFloat(t.barStrokeWidth)||0)+((parseFloat(t.barWidth)||0)+5)*i:0)}function P(e,t){te.prepareTicks(t);var i=$(C(e,t)),r=void 0,n=void 0,o=t.majorTicks.length,a=H.pixelRatio;for(e.lineWidth=2*a,e.save(),n=t.colorMajorTicks instanceof Array?t.colorMajorTicks:new Array(o).fill(t.colorStrokeTicks||t.colorMajorTicks),r=0;r<o;++r)e.strokeStyle=n[r],e.rotate(te.radians(N(t,t.exactTicks?t.majorTicks[r]:r,o))),e.beginPath(),e.moveTo(0,i),e.lineTo(0,i-.15*e.max),j(e);t.strokeTicks&&(e.strokeStyle=t.colorStrokeTicks||n[0],e.rotate(re),e.beginPath(),e.arc(0,0,i,te.radians(t.startAngle),te.radians(t.startAngle+t.ticksAngle),!1),j(e))}function N(e,t,i){if(e.exactTicks){var r=e.ticksAngle/(e.maxValue-e.minValue);return e.startAngle+r*(t-e.minValue)}return e.startAngle+t*(e.ticksAngle/(i-1))}function j(e){e.stroke(),e.restore(),e.closePath(),e.save()}function M(e,t){var i=C(e,t)-.15*e.max,r={},n=0,o=t.majorTicks.length,a="needle"!==t.animationTarget,l=t.colorNumbers instanceof Array?t.colorNumbers:new Array(o).fill(t.colorNumbers),s=a?-(t.value-t.minValue)/(t.maxValue-t.minValue)*t.ticksAngle:0;for(a&&(e.save(),e.rotate(-te.radians(s))),e.font=te.font(t,"Numbers",e.max/200),e.lineWidth=0,e.textAlign="center",e.textBaseline="middle";n<o;++n){var d=s+N(t,t.exactTicks?t.majorTicks[n]:n,o),u=e.measureText(t.majorTicks[n]).width,c=t.fontNumbersSize,h=Math.sqrt(u*u+c*c)/2,f=te.radialPoint(i-h-t.numbersMargin/100*e.max,te.radians(d));360===d&&(d=0),r[d]||(r[d]=!0,e.fillStyle=l[n],e.fillText(t.majorTicks[n],f.x,f.y))}a&&e.restore()}function B(e,t){t.title&&(e.save(),e.font=te.font(t,"Title",e.max/200),e.fillStyle=t.colorTitle,e.textAlign="center",e.fillText(t.title,0,-e.max/4.25,.8*e.max),e.restore())}function W(e,t){t.units&&(e.save(),e.font=te.font(t,"Units",e.max/200),e.fillStyle=t.colorUnits,e.textAlign="center",e.fillText(te.formatContext(t,t.units),0,e.max/3.25,.8*e.max),e.restore())}function _(e,t){if(t.needle){var i=t.ticksAngle<360?te.normalizedValue(t).indented:t.value,r=h?t.startAngle:t.startAngle+(i-t.minValue)/(t.maxValue-t.minValue)*t.ticksAngle;"right"===t.barStartPosition&&(r=t.startAngle+t.ticksAngle-(i-t.minValue)/(t.maxValue-t.minValue)*t.ticksAngle);var n=S(e,t),o=$(n/100*t.needleCircleSize),a=$(n/100*t.needleCircleSize*.75),l=$(n/100*t.needleEnd),s=$(t.needleStart?n/100*t.needleStart:0),d=n/100*t.needleWidth,u=n/100*t.needleWidth/2,c=H.pixelRatio,h="needle"!==t.animationTarget;e.save(),te.drawNeedleShadow(e,t),e.rotate(te.radians(r)),e.fillStyle=te.linearGradient(e,t.colorNeedle,t.colorNeedleEnd,l-s),"arrow"===t.needleType?(e.beginPath(),e.moveTo(-u,-s),e.lineTo(-d,0),e.lineTo(-1*c,l),e.lineTo(c,l),e.lineTo(d,0),e.lineTo(u,-s),e.closePath(),e.fill(),e.beginPath(),e.lineTo(-.5*c,l),e.lineTo(-1*c,l),e.lineTo(-d,0),e.lineTo(-u,-s),e.lineTo(u/2*c-2*c,-s),e.closePath(),e.fillStyle=t.colorNeedleShadowUp,e.fill()):(e.beginPath(),e.moveTo(-u,l),e.lineTo(-u,s),e.lineTo(u,s),e.lineTo(u,l),e.closePath(),e.fill()),t.needleCircleSize&&(e.restore(),te.drawNeedleShadow(e,t),t.needleCircleOuter&&(e.beginPath(),e.arc(0,0,o,0,2*ie,!0),e.fillStyle=te.linearGradient(e,t.colorNeedleCircleOuter,t.colorNeedleCircleOuterEnd,o),e.fill(),e.closePath()),t.needleCircleInner&&(e.beginPath(),e.arc(0,0,a,0,2*ie,!0),e.fillStyle=te.linearGradient(e,t.colorNeedleCircleInner,t.colorNeedleCircleInnerEnd,a),e.fill(),e.closePath()),e.restore())}}function E(e,t,i){te.drawValueBox(e,t,i,0,e.max-.33*e.max,e.max)}function R(e){var t=e.startAngle,i=e.startAngle+e.ticksAngle,r=t,n=t+(te.normalizedValue(e).normal-e.minValue)/(e.maxValue-e.minValue)*e.ticksAngle;if("middle"===e.barStartPosition){var o=.5*(e.minValue+e.maxValue);e.value<o?(r=180-(o-te.normalizedValue(e).normal)/(e.maxValue-e.minValue)*e.ticksAngle,n=180):(r=180,n=180+(te.normalizedValue(e).normal-o)/(e.maxValue-e.minValue)*e.ticksAngle)}else"right"===e.barStartPosition&&(r=i-n+t,n=i);return{startAngle:r,endAngle:n}}function I(e,t){var i=e.max/100,r=S(e,t)-5*i,n=parseFloat(t.barStrokeWidth+"")||0,o=(parseFloat(t.barWidth+"")||0)*i,a=r-2*n-o,l=(r-a)/2,s=a+l,d=n/s,u=t.startAngle,c=t.startAngle+t.ticksAngle;if(e.save(),e.rotate(re),n&&(e.beginPath(),e.arc(0,0,s,te.radians(u)-d,te.radians(c)+d,!1),e.strokeStyle=t.colorBarStroke,e.lineWidth=2*l,e.stroke(),e.closePath()),o&&(e.beginPath(),e.arc(0,0,s,te.radians(u),te.radians(c),!1),e.strokeStyle=t.colorBar,e.lineWidth=o,e.stroke(),e.closePath(),t.barShadow&&(e.beginPath(),e.arc(0,0,r,te.radians(u),te.radians(c),!1),e.clip(),e.beginPath(),e.strokeStyle=t.colorBar,e.lineWidth=1,e.shadowBlur=t.barShadow,e.shadowColor=t.colorBarShadow,e.shadowOffsetX=0,e.shadowOffsetY=0,e.arc(0,0,r,te.radians(t.startAngle),te.radians(t.startAngle+t.ticksAngle),!1),e.stroke(),e.closePath(),e.restore(),e.rotate(re)),t.barProgress)){var h=R(t),f=h.startAngle,v=h.endAngle;e.beginPath(),e.arc(0,0,s,te.radians(f),te.radians(v),!1),e.strokeStyle=t.colorBarProgress,e.lineWidth=o,e.stroke(),e.closePath()}e.restore()}function z(e){return e.options.animatedValue?e.options.value:e.value}var D=function e(t,i,r){null===t&&(t=Function.prototype);var n=Object.getOwnPropertyDescriptor(t,i);if(void 0===n){var o=Object.getPrototypeOf(t);return null===o?void 0:e(o,i,r)}if("value"in n)return n.value;var a=n.get;if(void 0!==a)return a.call(r)},F=function e(t,i,r,n){var o=Object.getOwnPropertyDescriptor(t,i);if(void 0===o){var a=Object.getPrototypeOf(t);null!==a&&e(a,i,r,n)}else if("value"in o&&o.writable)o.value=r;else{var l=o.set;void 0!==l&&l.call(n,r)}return r},G=function(){function e(e,t){for(var i=0;i<t.length;i++){var r=t[i];r.enumerable=r.enumerable||!1,r.configurable=!0,"value"in r&&(r.writable=!0),Object.defineProperty(e,r.key,r)}}return function(t,i,r){return i&&e(t.prototype,i),r&&e(t,r),t}}();Object.assign||Object.defineProperty(Object,"assign",{enumerable:!1,configurable:!0,writable:!0,value:function(e,t){if(void 0===e||null===e)throw new TypeError("Cannot convert first argument to object");for(var i=Object(e),r=1;r<arguments.length;r++){var n=arguments[r];if(void 0!==n&&null!==n)for(var o=Object.keys(Object(n)),a=0,l=o.length;a<l;a++){var s=o[a],d=Object.getOwnPropertyDescriptor(n,s);void 0!==d&&d.enumerable&&(i[s]=n[s])}}return i}}),Array.prototype.indexOf||Object.defineProperty(Array.prototype,"indexOf",{value:function(e,t){var i;if(null===this)throw new TypeError('"this" is null or not defined');var r=Object(this),n=r.length>>>0;if(0===n)return-1;var o=+t||0;if(Math.abs(o)===1/0&&(o=0),o>=n)return-1;for(i=Math.max(o>=0?o:n-Math.abs(o),0);i<n;){if(i in r&&r[i]===e)return i;i++}return-1}}),Array.prototype.fill||Object.defineProperty(Array.prototype,"fill",{value:function(e){if(null===this)throw new TypeError("this is null or not defined");for(var t=Object(this),i=t.length>>>0,r=arguments[1],n=r>>0,o=n<0?Math.max(i+n,0):Math.min(n,i),a=arguments[2],l=void 0===a?i:a>>0,s=l<0?Math.max(i+l,0):Math.min(l,i);o<s;)t[o]=e,o++;return t}}),"undefined"==typeof window&&(window="undefined"==typeof global?{}:global);var L=function(){function e(){r(this,e),this._events={},this.addListener=this.on,this.removeListener=this.off}return G(e,[{key:"emit",value:function(e){if(this._events[e]){for(var t=0,i=this._events[e].length,r=arguments.length,n=Array(r>1?r-1:0),o=1;o<r;o++)n[o-1]=arguments[o];for(;t<i;t++)this._events[e][t]&&this._events[e][t].apply(this,n)}}},{key:"once",value:function(e){for(var t=arguments.length,i=Array(t>1?t-1:0),r=1;r<t;r++)i[r-1]=arguments[r];for(var n=0,o=i.length,a=this;n<o;n++)!function(){var t=i[n],r=function i(){a.off(e,i),t.apply(a,arguments)};i[n]=r}();this.on.apply(this,[e].concat(i))}},{key:"on",value:function(e){this._events[e]||(this._events[e]=[]);for(var t=0,i=arguments.length<=1?0:arguments.length-1;t<i;t++)this._events[e].push(arguments.length<=t+1?void 0:arguments[t+1])}},{key:"off",value:function(e){if(this._events[e])for(var t=0,i=arguments.length<=1?0:arguments.length-1;t<i;t++)for(var r=arguments.length<=t+1?void 0:arguments[t+1],n=void 0;~(n=this._events[e].indexOf(r));)this._events[e].splice(n,1)}},{key:"removeAllListeners",value:function(e){delete this._events[e]}},{key:"listeners",get:function(){return this._events}}]),e}(),U=n("requestAnimationFrame")||function(e){return setTimeout(function(){return e((new Date).getTime())},1e3/60)},q={linear:function(e){return e},quad:function(e){return Math.pow(e,2)},dequad:function(e){return 1-q.quad(1-e)},quint:function(e){return Math.pow(e,5)},dequint:function(e){return 1-Math.pow(1-e,5)},cycle:function(e){return 1-Math.sin(Math.acos(e))},decycle:function(e){return Math.sin(Math.acos(1-e))},bounce:function(e){return 1-q.debounce(1-e)},debounce:function(e){for(var t=0,i=1;1;t+=i,i/=2)if(e>=(7-4*t)/11)return-Math.pow((11-6*t-11*e)/4,2)+Math.pow(i,2)},elastic:function(e){return 1-q.delastic(1-e)},delastic:function(e){return Math.pow(2,10*(e-1))*Math.cos(20*Math.PI*1.5/3*e)}},X=function(){function e(){var t=arguments.length>0&&void 0!==arguments[0]?arguments[0]:"linear",i=arguments.length>1&&void 0!==arguments[1]?arguments[1]:250,n=arguments.length>2&&void 0!==arguments[2]?arguments[2]:function(){},o=arguments.length>3&&void 0!==arguments[3]?arguments[3]:function(){};if(r(this,e),this.duration=i,this.rule=t,this.draw=n,this.end=o,"function"!=typeof this.draw)throw new TypeError("Invalid animation draw callback:",n);if("function"!=typeof this.end)throw new TypeError("Invalid animation end callback:",o)}return G(e,[{key:"animate",value:function(e,t){var i=this;this.frame&&this.cancel();var r=window.performance&&window.performance.now?window.performance.now():n("animationStartTime")||Date.now();e=e||this.draw,t=t||this.end,this.draw=e,this.end=t,this.frame=U(function(n){return o(n,e,r,q[i.rule]||i.rule,i.duration,t,i)})}},{key:"cancel",value:function(){if(this.frame){(n("cancelAnimationFrame")||function(e){})(this.frame),this.frame=null}}},{key:"destroy",value:function(){this.cancel(),this.draw=null,this.end=null}}]),e}();X.rules=q;var Y=function(){function t(i,n,o){r(this,t),this.options=i,this.element=n.toLowerCase(),this.type=t.toDashed(o),this.Type=e[o],this.mutationsObserved=!1,this.isObservable=!!window.MutationObserver,window.GAUGES_NO_AUTO_INIT||t.domReady(this.traverse.bind(this))}return G(t,[{key:"isValidNode",value:function(e){return!(!e.tagName||e.tagName.toLowerCase()!==this.element||e.getAttribute("data-type")!==this.type)}},{key:"traverse",value:function(){for(var e=document.getElementsByTagName(this.element),t=0,i=e.length;t<i;t++)this.process(e[t]);this.isObservable&&!this.mutationsObserved&&(new MutationObserver(this.observe.bind(this)).observe(document.body,{childList:!0,subtree:!0,attributes:!0,characterData:!0,attributeOldValue:!0,characterDataOldValue:!0}),this.mutationsObserved=!0)}},{key:"observe",value:function(e){for(var t=0,i=e.length;t<i;t++){var r=e[t];if("attributes"===r.type&&"data-type"===r.attributeName&&this.isValidNode(r.target)&&r.oldValue!==this.type)setTimeout(this.process.bind(this,r.target));else if(r.addedNodes&&r.addedNodes.length)for(var n=0,o=r.addedNodes.length;n<o;n++)setTimeout(this.process.bind(this,r.addedNodes[n]))}}},{key:"process",value:function(e){var i=this;if(!this.isValidNode(e))return null;var r=void 0,n=JSON.parse(JSON.stringify(this.options)),o=null;for(r in n)if(n.hasOwnProperty(r)){var a=t.toAttributeName(r),l=t.parse(e.getAttribute(a));null!==l&&void 0!==l&&(n[r]=l)}return n.renderTo=e,o=new this.Type(n),o.draw&&o.draw(),this.isObservable?(o.observer=new MutationObserver(function(r){r.forEach(function(r){if("attributes"===r.type){var n=r.attributeName.toLowerCase(),a=e.getAttribute(n).toLowerCase();if("data-type"===n&&a&&a!==i.type)o.observer.disconnect(),delete o.observer,o.destroy&&o.destroy();else if("data-"===n.substr(0,5)){var l=n.substr(5).split("-").map(function(e,t){return t?e.charAt(0).toUpperCase()+e.substr(1):e}).join(""),s={};s[l]=t.parse(e.getAttribute(r.attributeName)),"value"===l?o&&(o.value=s[l]):o.update&&o.update(s)}}})}),o.observer.observe(e,{attributes:!0}),o):o}}],[{key:"parse",value:function(e){if("true"===e)return!0;if("false"===e)return!1;if("undefined"!==e){if("null"===e)return null;if(/^[-+#.\w\d\s]+(?:,[-+#.\w\d\s]*)+$/.test(e))return e.split(",");try{return JSON.parse(e)}catch(e){}return e}}},{key:"toDashed",value:function(e){for(var t=e.split(/(?=[A-Z])/),i=1,r=t.length,n=t[0].toLowerCase();i<r;i++)n+="-"+t[i].toLowerCase();return n}},{key:"toCamelCase",value:function(e){for(var t=!(arguments.length>1&&void 0!==arguments[1])||arguments[1],i=e.split(/-/),r=0,n=i.length,o="";r<n;r++)o+=r||t?i[r][0].toUpperCase()+i[r].substr(1).toLowerCase():i[r].toLowerCase();return o}},{key:"toAttributeName",value:function(e){return"data-"+t.toDashed(e)}},{key:"domReady",value:function(e){if(/comp|inter|loaded/.test((window.document||{}).readyState+""))return e();window.addEventListener?window.addEventListener("DOMContentLoaded",e,!1):window.attachEvent&&window.attachEvent("onload",e)}}]),t}(),H=function(){function e(t,i,n){r(this,e),e.collection.push(this),this.width=i||0,this.height=n||0,this.element=t,this.init()}return G(e,[{key:"init",value:function(){var t=e.pixelRatio;this.element.width=this.width*t,this.element.height=this.height*t,this.element.style.width=this.width+"px",this.element.style.height=this.height+"px",this.elementClone=this.element.cloneNode(!0),this.context=this.element.getContext("2d"),this.contextClone=this.elementClone.getContext("2d"),this.drawWidth=this.element.width,this.drawHeight=this.element.height,this.drawX=this.drawWidth/2,this.drawY=this.drawHeight/2,this.minSide=this.drawX<this.drawY?this.drawX:this.drawY,this.elementClone.initialized=!1,this.contextClone.translate(this.drawX,this.drawY),this.contextClone.save(),this.context.translate(this.drawX,this.drawY),this.context.save(),this.context.max=this.contextClone.max=this.minSide,this.context.maxRadius=this.contextClone.maxRadius=null}},{key:"destroy",value:function(){var t=e.collection.indexOf(this);~t&&e.collection.splice(t,1),this.context.clearRect(-this.drawX,-this.drawY,this.drawWidth,this.drawHeight),this.context.max=null,delete this.context.max,this.context.maxRadius=null,delete this.context.maxRadius,this.context=null,this.contextClone=null,this.elementClone=null,this.element=null,this.onRedraw=null}},{key:"commit",value:function(){var t=e.pixelRatio;return 1!==t&&(this.contextClone.scale(t,t),this.contextClone.save()),this}},{key:"redraw",value:function(){return this.init(),this.onRedraw&&this.onRedraw(),this}}],[{key:"redraw",value:function(){for(var t=0,i=e.collection.length;t<i;t++)e.collection[t].redraw()}},{key:"pixelRatio",get:function(){return window.devicePixelRatio||1}}]),e}();H.collection=[],window.matchMedia&&window.matchMedia("screen and (min-resolution: 2dppx)").addListener(H.redraw);var J={renderTo:null,width:0,height:0,minValue:0,maxValue:100,value:0,units:!1,exactTicks:!1,majorTicks:[0,20,40,60,80,100],minorTicks:10,strokeTicks:!0,animatedValue:!1,animateOnInit:!1,title:!1,borders:!0,numbersMargin:1,listeners:null,valueInt:3,valueDec:2,majorTicksInt:1,majorTicksDec:0,animation:!0,animationDuration:500,animationRule:"cycle",colorPlate:"#fff",colorPlateEnd:"",colorMajorTicks:"#444",colorMinorTicks:"#666",colorStrokeTicks:"",colorTitle:"#888",colorUnits:"#888",colorNumbers:"#444",colorNeedle:"rgba(240,128,128,1)",colorNeedleEnd:"rgba(255,160,122,.9)",colorValueText:"#444",colorValueTextShadow:"rgba(0,0,0,0.3)",colorBorderShadow:"rgba(0,0,0,0.5)",colorBorderOuter:"#ddd",colorBorderOuterEnd:"#aaa",colorBorderMiddle:"#eee",colorBorderMiddleEnd:"#f0f0f0",colorBorderInner:"#fafafa",colorBorderInnerEnd:"#ccc",colorValueBoxRect:"#888",colorValueBoxRectEnd:"#666",colorValueBoxBackground:"#babab2",colorValueBoxShadow:"rgba(0,0,0,1)",colorNeedleShadowUp:"rgba(2,255,255,0.2)",colorNeedleShadowDown:"rgba(188,143,143,0.45)",colorBarStroke:"#222",colorBar:"#ccc",colorBarProgress:"#888",colorBarShadow:"#000",fontNumbers:"Arial",fontTitle:"Arial",fontUnits:"Arial",fontValue:"Arial",fontNumbersSize:20,fontTitleSize:24,fontUnitsSize:22,fontValueSize:26,fontNumbersStyle:"normal",fontTitleStyle:"normal",fontUnitsStyle:"normal",fontValueStyle:"normal",fontNumbersWeight:"normal",fontTitleWeight:"normal",fontUnitsWeight:"normal",fontValueWeight:"normal",needle:!0,needleShadow:!0,needleType:"arrow",needleStart:5,needleEnd:85,needleWidth:4,borderOuterWidth:3,borderMiddleWidth:3,borderInnerWidth:3,borderShadowWidth:3,valueBox:!0,valueBoxStroke:5,valueBoxWidth:0,valueText:"",valueTextShadow:!0,valueBoxBorderRadius:2.5,highlights:[{from:20,to:60,color:"#eee"},{from:60,to:80,color:"#ccc"},{from:80,to:100,color:"#999"}],highlightsWidth:15,highlightsLineCap:"butt",barWidth:20,barStrokeWidth:0,barProgress:!0,barShadow:0};a.prototype=Object.create(Array.prototype),a.prototype.constructor=a,a.prototype.get=function(e){if("string"==typeof e)for(var t=0,i=this.length;t<i;t++){var r=this[t].options.renderTo.tagName?this[t].options.renderTo:document.getElementById(this[t].options.renderTo||"");if(r.getAttribute("id")===e)return this[t]}else if("number"==typeof e)return this[e];return null};var Z="2.1.7",$=(Math.round,Math.abs),K=new a;K.version=Z;var Q=function(n){function o(i){r(this,o);var n=t(this,(o.__proto__||Object.getPrototypeOf(o)).call(this)),a=n.constructor.name;if("BaseGauge"===a)throw new TypeError("Attempt to instantiate abstract class!");if(K.push(n),i.listeners&&Object.keys(i.listeners).forEach(function(e){(i.listeners[e]instanceof Array?i.listeners[e]:[i.listeners[e]]).forEach(function(t){n.on(e,t)})}),n.version=Z,n.type=e[a]||o,n.initialized=!1,i.minValue=parseFloat(i.minValue),i.maxValue=parseFloat(i.maxValue),i.value=parseFloat(i.value)||0,i.borders||(i.borderInnerWidth=i.borderMiddleWidth=i.borderOuterWidth=0),!i.renderTo)throw TypeError("Canvas element was not specified when creating the Gauge object!");var l=i.renderTo.tagName?i.renderTo:document.getElementById(i.renderTo);if(!(l instanceof HTMLCanvasElement))throw TypeError("Given gauge canvas element is invalid!");return i.width=parseFloat(i.width)||0,i.height=parseFloat(i.height)||0,i.width&&i.height||(i.width||(i.width=l.parentNode?l.parentNode.offsetWidth:l.offsetWidth),i.height||(i.height=l.parentNode?l.parentNode.offsetHeight:l.offsetHeight)),n.options=i||{},n.options.animateOnInit&&(n._value=n.options.value,n.options.value=n.options.minValue),n.canvas=new H(l,i.width,i.height),n.canvas.onRedraw=n.draw.bind(n),n.animation=new X(i.animationRule,i.animationDuration),n}return i(o,n),G(o,[{key:"update",value:function(e){return Object.assign(this.options,this.type.configure(e||{})),this.canvas.width=this.options.width,this.canvas.height=this.options.height,this.animation.rule=this.options.animationRule,this.animation.duration=this.options.animationDuration,this.canvas.redraw(),this}},{key:"destroy",value:function(){var e=K.indexOf(this);~e&&K.splice(e,1),this.canvas.destroy(),this.canvas=null,this.animation.destroy(),this.animation=null,this.emit("destroy")}},{key:"draw",value:function(){return this.options.animateOnInit&&!this.initialized&&(this.value=this._value,this.initialized=!0,this.emit("init")),this.emit("render"),this}},{key:"value",set:function(e){var t=this;e=o.ensureValue(e,this.options.minValue);var i=this.options.value;if(e!==i)if(this.options.animation){if(this.animation.frame&&(this.options.value=this._value,this._value===e))return this.animation.cancel(),void delete this._value;void 0===this._value&&(this._value=e),this.emit("animationStart"),this.animation.animate(function(r){var n=i+(e-i)*r;t.options.animatedValue&&t.emit("value",n,t.value),t.options.value=n,t.draw(),t.emit("animate",r,t.options.value)},function(){void 0!==t._value&&(t.emit("value",t._value,t.value),t.options.value=t._value,delete t._value),t.draw(),t.emit("animationEnd")})}else this.emit("value",e,this.value),this.options.value=e,this.draw()},get:function(){return void 0===this._value?this.options.value:this._value}}],[{key:"configure",value:function(e){return e}},{key:"initialize",value:function(e,t){return new Y(t,"canvas",e)}},{key:"fromElement",value:function(e){var t=Y.toCamelCase(e.getAttribute("data-type")),i=e.attributes,r=0,n=i.length,o={};if(t){for(/Gauge$/.test(t)||(t+="Gauge");r<n;r++)o[Y.toCamelCase(i[r].name.replace(/^data-/,""),!1)]=Y.parse(i[r].value);new Y(o,e.tagName,t).process(e)}}},{key:"ensureValue",value:function(e){var t=arguments.length>1&&void 0!==arguments[1]?arguments[1]:0;return e=parseFloat(e),!isNaN(e)&&isFinite(e)||(e=parseFloat(t)||0),e}},{key:"mod",value:function(e,t){return(e%t+t)%t}},{key:"version",get:function(){return Z}}]),o}(L);void 0!==e&&(e.BaseGauge=Q,e.gauges=(window.document||{}).gauges=K);var ee=/{([_a-zA-Z]+[_a-zA-Z0-9]*)}/g,te={roundRect:u,padValue:c,formatMajorTickNumber:h,radians:f,radialPoint:v,linearGradient:m,drawNeedleShadow:p,drawValueBox:x,verifyError:l,prepareTicks:d,drawShadow:g,font:w,normalizedValue:k,formatContext:s},ie=Math.PI,re=ie/2,ne=Object.assign({},J,{ticksAngle:270,startAngle:45,colorNeedleCircleOuter:"#f0f0f0",colorNeedleCircleOuterEnd:"#ccc",colorNeedleCircleInner:"#e8e8e8",colorNeedleCircleInnerEnd:"#f5f5f5",needleCircleSize:10,needleCircleInner:!0,needleCircleOuter:!0,needleStart:20,animationTarget:"needle",useMinPath:!1,barWidth:0,barStartPosition:"left"}),oe=function(e){function n(e){return r(this,n),e=Object.assign({},ne,e||{}),t(this,(n.__proto__||Object.getPrototypeOf(n)).call(this,n.configure(e)))}return i(n,e),G(n,[{key:"draw",value:function(){try{var e=this.canvas,t=[-e.drawX,-e.drawY,e.drawWidth,e.drawHeight],i=t[0],r=t[1],o=t[2],a=t[3],l=this.options;if("needle"===l.animationTarget){if(!e.elementClone.initialized){var s=e.contextClone;s.clearRect(i,r,o,a),s.save(),this.emit("beforePlate"),O(s,l),this.emit("beforeHighlights"),A(s,l),this.emit("beforeMinorTicks"),V(s,l),this.emit("beforeMajorTicks"),P(s,l),this.emit("beforeNumbers"),M(s,l),this.emit("beforeTitle"),B(s,l),this.emit("beforeUnits"),W(s,l),e.elementClone.initialized=!0}this.canvas.commit(),e.context.clearRect(i,r,o,a),e.context.save(),e.context.drawImage(e.elementClone,i,r,o,a),e.context.save(),this.emit("beforeProgressBar"),I(e.context,l),this.emit("beforeValueBox"),E(e.context,l,z(this)),this.emit("beforeNeedle"),_(e.context,l)}else{var d=-te.radians((l.value-l.minValue)/(l.maxValue-l.minValue)*l.ticksAngle);if(e.context.clearRect(i,r,o,a),e.context.save(),this.emit("beforePlate"),O(e.context,l),e.context.rotate(d),this.emit("beforeHighlights"),A(e.context,l),this.emit("beforeMinorTicks"),V(e.context,l),this.emit("beforeMajorTicks"),P(e.context,l),this.emit("beforeNumbers"),M(e.context,l),this.emit("beforeProgressBar"),I(e.context,l),e.context.rotate(-d),e.context.save(),!e.elementClone.initialized){var u=e.contextClone;u.clearRect(i,r,o,a),u.save(),this.emit("beforeTitle"),B(u,l),this.emit("beforeUnits"),W(u,l),this.emit("beforeNeedle"),_(u,l),e.elementClone.initialized=!0}e.context.drawImage(e.elementClone,i,r,o,a)}this.emit("beforeValueBox"),E(e.context,l,z(this)),D(n.prototype.__proto__||Object.getPrototypeOf(n.prototype),"draw",this).call(this)}catch(e){te.verifyError(e)}return this}},{key:"value",set:function(e){e=Q.ensureValue(e,this.options.minValue),this.options.animation&&360===this.options.ticksAngle&&this.options.useMinPath&&(this._value=e,e=this.options.value+((e-this.options.value)%360+540)%360-180),F(n.prototype.__proto__||Object.getPrototypeOf(n.prototype),"value",e,this)},get:function(){return D(n.prototype.__proto__||Object.getPrototypeOf(n.prototype),"value",this)}}],[{key:"configure",value:function(e){return e.barWidth>50&&(e.barWidth=50),isNaN(e.startAngle)&&(e.startAngle=45),isNaN(e.ticksAngle)&&(e.ticksAngle=270),e.ticksAngle>360&&(e.ticksAngle=360),e.ticksAngle<0&&(e.ticksAngle=0),e.startAngle<0&&(e.startAngle=0),e.startAngle>360&&(e.startAngle=360),e}}]),n}(Q);void 0!==e&&(e.RadialGauge=oe),Q.initialize("RadialGauge",ne),"undefined"!=typeof module&&Object.assign(e,{Collection:a,GenericOptions:J,Animation:X,BaseGauge:Q,drawings:te,SmartCanvas:H,DomObserver:Y,vendorize:n})}("undefined"!=typeof module?module.exports:window);
  </script> 
  <script src="script.js"></script>
</body>
</html>

)rawliteral";

const char* css = R"rawliteral(

/* General Styles */
body {
    margin: 0;
    font-family: 'Arial', sans-serif;
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    background-color: #121212;
    color: #e0e0e0;
}

@keyframes flash {
    0% { background-color: #121212;}
    50% { background-color: rgb(226, 76, 76);}
    100% { background-color: #121212;}

}

.flash-bg {
    animation: flash 1s infinite;
}

@media (max-width: 1200px) {
    .desktop-only {
      display: none;
    }
}

.container {
    margin: auto;
    display: flex;
    width: 90%;
    height: 80%;
    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.5);
    border-radius: 10px;
    overflow: scroll;
    background-color: #1e1e1e;
}

.slidecontainer {
    width: 75%; /* Width of the outside container */
    height: 5px;
    display: flex;
    flex-direction: column;
  }
  
  /* The slider itself */
.slider {
    align-self: center;
    -webkit-appearance: none;  /* Override default CSS styles */
    appearance: none;
    width: 100%; /* Full-width */
    height: 15px; /* Specified height */
    background: #d3d3d3; /* Grey background */
    outline: none; /* Remove outline */
    opacity: 0.7; /* Set transparency (for mouse-over effects on hover) */
    -webkit-transition: .2s; /* 0.2 seconds transition on hover */
    transition: opacity .2s;
  }
  
  /* Mouse-over effects */
.slider:hover {
    opacity: 0.5; /* Fully shown on mouse-over */
  }
  
  /* The slider handle (use -webkit- (Chrome, Opera, Safari, Edge) and -moz- (Firefox) to override default look) */
.slider::-webkit-slider-thumb {
    -webkit-appearance: none; /* Override default look */
    appearance: none;
    width: 15%; /* Set a specific slider handle width */
    height: 15px; /* Slider handle height */
    background: #00cc66; /* Green background */
    cursor: pointer; /* Cursor on hover */
  }
  
.slider::-moz-range-thumb {
    width: 15px;
    height: 25px;
    background: #177bff;
    cursor: pointer;
  }

/* Left Panel */
.left-panel {
    flex: 2;
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    background: #1e1e1e;
    padding: 20px;
}

.controls {
    display: flex;
    flex-direction: column;
    gap: 20px;
}

.buttons {
    display: flex;
    flex-direction: column;
    gap: 15px;
}

.start {
    --default-color: #00cc66;
    --hover-color: #028a3f;
    background-color: var(--default-color);
    color: #fff;
    border: none;
    border-radius: 8px;
    padding: 15px 25px;
    font-size: 16px;
    cursor: pointer;
    transition: background-color 0.3s ease;
}

.start:hover {
    background-color: var(--hover-color);
}

.changemode {
    background-color: #838074;
    color: #fff;
    border: none;
    border-radius: 8px;
    padding: 15px 25px;
    font-size: 16px;
    cursor: pointer;
    transition: background-color 0.3s ease;
    margin-top: 10px;
}

.changemode:hover {
    background-color: #5f5a52;
}

button {
    padding: 15px 25px;
    font-size: 16px;
    width: 40%;
    align-self: center;
    cursor: pointer;
    color: #fff;
    border: none;
    border-radius: 8px;
    transition: background-color 0.3s ease;
}

/* Speedometer Section */
.speedometer-section {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 20px;
}

#gaugeCanvas {
    background: transparent;
    display: block;
}

input[type="range"] {
    width: 80%;
    cursor: pointer;
}

input[type="range"]::-webkit-slider-thumb {
    appearance: none;
    height: 15px;
    width: 15px;
    background: #0066cc;
    border-radius: 50%;
    cursor: pointer;
}

/* Distance and Travelled displayed side by side */
.distance-container {
    align-self: center;
    display: flex;
    justify-content: space-between;
    width: 60%;
}

.distance-section {
    flex: 1;
    text-align: center;
}

/* Debug Panel */
.debug-panel {
    background: #2c2c2c;
    padding: 15px;
    border-radius: 5px;
    border-top: 2px solid #444;
    box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.5);
}

.debug-panel h3 {
    margin: 0 0 10px 0;
    font-size: 18px;
    color: #ddd;
}

.debug-panel p {
    font-size: 14px;
    margin: 15px 0;
}

.debug-panel span {
    font-weight: bold;
    color: #0066cc;
}

/* Right Panel (split into top and bottom sections) */
.right-panel {
    flex: 3;
    display: flex;
    flex-direction: column;
    background: #2a2a2a;
    color: #bbb;
    font-style: italic;
    font-size: 18px;
}

.top-right {
    /* flex: 1; */
    display: flex;
    height: 30%;
    justify-content: center;
    align-items: center;
}

.bottom-right {
    flex: 1;
    display: flex;

}

.top-right {
    border-bottom: 2px solid #444;
}

/* Directional Indicator inside the top-right section */
.direction-indicator {
    font-size: 24px;
}

.mobile-only {
    display: none;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    gap: 20px;
}

@media (max-width: 1200px) {
    .right-panel {
        display: none;
    }

    .mobile-only{
        display: flex;
    }
}

.images {
    src: "";
}

.canvas-container {
    padding: 0;
    margin: auto;
    display: block;
    width: 750px;
    height: 400px;;
    /* justify-content: center;
    align-items: center; */
}

)rawliteral";

const char* js = R"rawliteral(


//////////////////////////////////

// Variables
let enable = false;
let speed = 0;
let buggyspeed = 0;
let distance = 0;
let travelled = 0;
let mode = 0;
let socket;
let id = 0;
let obstacleDetected = false;
let badspeed = 0;

let minX = Infinity;
let maxX = -Infinity
let minY = Infinity;
let maxY = -Infinity;

let x = 0;
let y = 0;

const alpha = 0.5;
const padding = 20;

let isChangingSpeedSlider = false;

//////////////////////////////////

// References to IDs
const startStopBtn = document.getElementById('startStopBtn');
const speedValueElem = document.getElementById('speedValue');
const distanceElem = document.getElementById('distance');
const modeSelect = document.getElementById('changeMode');
const obstacleText = document.getElementById('obstacleText');

const slider = document.getElementById("speedslider");
const output = document.getElementById("speedSet");

const imageElements = document.querySelectorAll('.images');

const trackPoints = [];

/* #region Images */

const images = [

    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAACXBIWXMAAAsTAAALEwEAmpwYAAABZklEQVR4nO3RwWkDARDFUJeWbtZl5Jim/RVMKggEot2VYI4Dw7zH4wK9juPzdRxf/31HPX4w9nzynlBEGAvFh7FQfBgLxYexUHwYC8WHsVB8GAvFh7FQfBgLxYexUHwYC8WHsVB8GAvFh7FQfBgLxYexUHwYuzuKEWN3RTFj7G4oZ8DYXVDOhLGro5wRY1dFOTPGroryF3EcH7995Hvnv+++bATiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiihuAfANk+xFHMdUshwAAAABJRU5ErkJggg==",

    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAACXBIWXMAAAsTAAALEwEAmpwYAAACaklEQVR4nO3csWoUURSH8RGxT6FFKit7wULtLESEgEUgPsI9ZwbDFmqRzgeI4DOY0tI0IWBAsbaztBfBTgTleEICNnazzmb/d+73wdS5yW/PZDe5d7puTWXfP8hSbq/r69M/GOH+M83u8YMRwfjjnoAIYQAihgGIGAYgYhiAiGEAIoYBiBgGIGIYgIhhACKGAYgYBiBiGICIYQAihgGIGAYgYhjnIO5Ps+/vV3OVcjeH4WaWcrWbG0btV5h9D7OjNNtLsxtguBiQ+4d0f5Rdd4nJcKnJ+Xh2W+M25VLT8ivNnk0+LfzO8P+dloMs5QoYLjUtb3Jn5zLT4VIor5gS17qylG1QXGhKzL7mYrEBikvdul7yjsulQH7k7u41UFznSvfnsp9NqvvjYt9vpdkQZodhFktNidmnSUGmRKl593v2/a1w/7LU913K5ioW1Pw/qHIYrof7tyVeiI8nB5kCpeYJ+VuaPVnitvWiW1Wtb3LIxWIjzH6PBHm92kU1vg0ozD6PBHm78kW1vFEuzN6PBHl3IQtrdStpuJ+MAnE/ubDFtbjZOpRBWjyOEOogrR3YiRpAWjrSFrWAtHLoM2oCaeFYdNQGMvcHB0SNIHN+tEbUCjLXh89EzSBzfDxT1A5yVpbyMN3vdDMo5gAypwIQrQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQscJsP8yOR1z7614zERERUddUp0GqheoX+IPyAAAAAElFTkSuQmCC",

    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAACXBIWXMAAAsTAAALEwEAmpwYAAAChklEQVR4nO3aoYtVQRTH8RHtIrLJYLKrSZvBIKhYXPwP5vzue2wSN1gWzAuC3Wa0aRHBBQ1mg9F/YIPJonLmLPclFyz37byd3935feGUl+bw4YZ376SkjhU5346cHx7/VTUrzO448FsoRCAFCKGQgRShcPQviFAIQYRCCCIUQhChEIIIhRBEKIQgQiEEEQohiFAIQYRCCCIUQhChEIIIhRCEFiV2drZisbi++gI3DHdnM8DTk4JQoITZtQCeu9kHN/tZY6ky8zl1lEjpXJg9cuBL6+VL7ygB3HCzr60XLr2jrJ4K4JkDf1ovWnpHib29C272uvVyZaZTFSW2t8+72dvWS5WZTzUUB161XqacFRDgwcmeDrPHrRcpZ2DqYAzDJTc7bL1MmflUwRhzs5etlykzn2oYsVxeduBX64XKjKcaxgrEbLf1QmXGUxVjzM2+rXUQM3ezdwEMMQz3u325iIoYYXZlTYwfkfPN1Pvrd1TEWB0s5ydrYBzGYnE19f6BCpUxxtzsxdTDBLBMvX/CxQYwxhx4M/Hp+Bs5X0w9X3LAhjDG3Oz9xAN9Tz1fA8IGMcbc7NPEJ+Rz6vWiHDaMMebAwcSDHaQer5LiFDDGBAIeDIGAC0Mg4MIQCLgwBAIuDIGAC0Mg4MIQCLgwxnr/H+JMGL2DOBtG1yDArcj5XmKrVxDaBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMhys303+zhh9lufWSmllFJKqfTfjgDXtYbk+I8vyQAAAABJRU5ErkJggg==",

    "data:image/png;base64,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",

    "data:image/png;base64,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"
];

/* #endregion */

//////////////////////////////////

var gaugeElement = document.getElementsByTagName('canvas')[0];
const canvas = document.getElementById('odometry');
const ctx = canvas.getContext('2d');

// Set up WebSocket connection
function setupWebSocket() {
    socket = new WebSocket("ws://" + window.location.hostname + ":81");
    
    // console logging
    socket.onopen = () => {
        console.log("Connected to WebSocket server.");
    };

    // Incoming WebSocket messages
    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);

        if (data.speed !== undefined) {
            speed = data.speed;
            slider.value = speed;
            output.innerHTML = `Speed:${speed} cm/s`;
        }

        if (data.buggyspeed !== undefined) {

            // badspeed = data.buggyspeed;
            // buggyspeed = alpha * buggyspeed + (1 - alpha) * badspeed;
            buggyspeed = data.buggyspeed;
            // console.log(data.buggyspeed);
        }

        if (data.distance !== undefined) {
            document.querySelectorAll('.distance-section p')[0].textContent = `${data.distance} cm`;
            // console.log(data.distance);
        }

        if (data.travelled !== undefined) {
            document.querySelectorAll('.distance-section p')[1].textContent = `${data.travelled.toFixed(2)} m`;
        }

        if (data.TagID !== undefined) {
            id = data.TagID;
            console.log("Tag ID:", id);
            document.querySelectorAll('.variables p')[3].textContent = `Tag ID: ${id}`;
        }

        if (data.mode !== undefined) {
            mode = data.mode;
            updateModeText(mode); // Update mode description based on mode
        }

        if (data.obstacle !== undefined) {
            obstacleDetected = data.obstacle;
            updateObstacleAlert(obstacleDetected);
            // console.log(data.obstacle);
            //toogle obstacle detection
        }

        if (data.enable !== undefined) {
            enable = data.enable;
            updateStartButton();    
            /*update start button base on message, syncs multiple clients so that they all have the same state for start and stop*/
        }

        if (data.renesas !== undefined) {
            console.log("Renesas Loop Time:", data.renesas);
            document.querySelectorAll('.variables p')[0].textContent = `Renesas Loop Time: ${data.renesas}`;
        }

        if (data.esp !== undefined) {
            console.log("ESP32 Loop Time:", data.esp);
            document.querySelectorAll('.variables p')[1].textContent = `ESP32 Loop Time: ${data.esp}`;
        }

        if (data.peak !== undefined) {
            console.log("Peak:", data.peak);
            document.querySelectorAll('.variables p')[2].textContent = `Peak ESP Loop Time: ${data.peak}`;
        }

        if (data.x !== undefined) {
            x = data.x; 
        }

        if (data.y !== undefined) {
            y = data.y;
            addPoint(x, -y);
        }

        gaugeElement.setAttribute('data-value', buggyspeed);
        updateImage();
    };

    // Handle WebSocket close
    socket.onclose = () => {
        console.log("Disconnected from WebSocket server.");
    };

    // Handle WebSocket errors
    socket.onerror = (error) => {
        console.log("WebSocket error: ", error);
    };
}

// Toggle Start/Stop Button
startStopBtn.addEventListener('click', () => {

    console.log("Sending command:", !enable ? "start" : "stop");
    updateStartButton();
    
    if (enable) {
        socket.send("stop");
    } else {
        socket.send("start");
    }

});

// listen to clicks on change mode
modeSelect.addEventListener('click', () => {
    console.log("Sending command:", "changeMode");
    socket.send("changeMode");
});

// listen for change to speed slider
slider.addEventListener('input', () => {
    isChangingSpeedSlider = true;  // Prevent WebSocket updates while changing
    socket.send(`speedValue:${slider.value}`);
    output.innerHTML = `Speed:${slider.value} cm/s`;
});

// User stopped changing
slider.addEventListener("change", function () {
    isChangingSpeedSlider = false;  // Allow WebSocket updates again
});

// Display Obstacle Detected Text
function updateObstacleAlert(obstacleDetected) {
    if (obstacleDetected) {
        document.body.classList.add('flash-bg');
    } else {
        document.body.classList.remove('flash-bg');
    }
}

// Update start/stop button state based on enable state
function updateStartButton() {
    if (enable) {
        startStopBtn.textContent = 'Stop';
        startStopBtn.style.setProperty('--default-color', 'rgb(196, 64, 64)');
        startStopBtn.style.setProperty('--hover-color', 'rgb(153, 51, 51)');
    } else {
        startStopBtn.textContent = 'Start';
        startStopBtn.style.setProperty('--default-color', 'rgb(0, 204, 102)');
        startStopBtn.style.setProperty('--hover-color', 'rgb(0, 153, 77)');
    }
}

// Update mode description text
function updateModeText(mode) {
    let modeText = "";
    switch (mode) {
        case 0:
            modeText = "Reference Speed";
            break;
        case 1:
            modeText = "Reference Object";
            break;
        default:
            modeText = "Unknown Mode"   ;
            break;
    }
    modeSelect.textContent = `${modeText}`;
}

function updateSpeedValue() {
    if (!isChangingSpeedSlider) {
        slider.value = speed;
    }
}

function updateImage() {

    if (obstacleDetected) {
        imageElements.forEach(image => {
            image.src = images[3];
          });

    } else if (!enable) {
        imageElements.forEach(image => {
            image.src = "";
          });

    } else {
        imageElements.forEach(image => {
            image.src = images[id];
          });
    }
}

function addPoint(x, y) {
    trackPoints.push({ x: x, y: y });

    trackPoints.forEach(p => {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
      });

    drawPath();
}

function drawPath() {
    // Clear the canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Check if we have any points
    if (trackPoints.length === 0) return;

    const effectiveWidth = canvas.width - 2 * padding;
    const effectiveHeight = canvas.height - 2 * padding;

    const scaleX = effectiveWidth / (maxX - minX);
    const scaleY = effectiveHeight / (maxY - minY);
    
    const scale = Math.min(scaleX, scaleY);
    
    const translateX = padding - (minX * scale);
    const translateY = padding - (minY * scale);

    ctx.save();

    ctx.translate(translateX, translateY);
    ctx.scale(scale, scale);
    
    // Draw the path
    ctx.beginPath();
    ctx.moveTo(trackPoints[0].x, trackPoints[0].y);
    trackPoints.forEach(point => {
      ctx.lineTo(point.x, point.y);
    });
    ctx.strokeStyle = 'white';
    ctx.lineWidth = 3 / scale;
    ctx.stroke();

    ctx.restore();
}

// updateModeText(mode); // might be redundant but havent tested
setupWebSocket();

)rawliteral";

#endif