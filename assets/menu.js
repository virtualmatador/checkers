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
    switch (difficulty) {
        case 2:
            document.getElementById("difficulty").value = 'Easy';
            break;
        case 4:
            document.getElementById("difficulty").value = 'Normal';
            break;
        case 6:
            document.getElementById("difficulty").value = 'Difficult';
            break;
        case 8:
            document.getElementById("difficulty").value = 'Master';
            break;
    }
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

function difficulty() {
    switch (document.getElementById("difficulty").value) {
        case 'Easy':
            CallHandler("difficulty", "click", "2");
            break;
        case 'Normal':
            CallHandler("difficulty", "click", "4");
            break;
        case 'Difficult':
            CallHandler("difficulty", "click", "6");
            break;
        case 'Master':
            CallHandler("difficulty", "click", "8");
            break;
    }
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
