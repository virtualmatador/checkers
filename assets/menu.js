function play() {
    CallHandler("play", "click", "");
}

function reset() {
    CallHandler("reset", "click", "");
}

function setAlter(state) {
    document.getElementById("alter").checked = state;
}

function setDifficulty(difficulty) {
    document.getElementById("difficulty").value = difficulty.toString();
}

function setRotate(state) {
    document.getElementById("rotate").checked = state;
}

function setSound(state) {
    document.getElementById("sound").checked = state;
}

function setThumb(state) {
    document.getElementById("thumb").checked = state;
}

function setHighlight(state) {
    document.getElementById("highlight").checked = state;
}

function difficulty() {
    CallHandler("difficulty", "click",
        document.getElementById("difficulty").value);
}

function alter() {
    CallHandler("alter", "click", document.getElementById("alter").checked.toString());
}

function rotate() {
    CallHandler("rotate", "click", document.getElementById("rotate").checked.toString());
}

function sound() {
    CallHandler("sound", "click", document.getElementById("sound").checked.toString());
}

function thumb() {
    CallHandler("thumb", "click", document.getElementById("thumb").checked.toString());
}

function highlight() {
    CallHandler("highlight", "click", document.getElementById("highlight").checked.toString());
}
