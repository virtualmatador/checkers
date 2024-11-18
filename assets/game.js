var alter_ = false;
var rotate_ = false;

var audios_ = [];
const AudioContext = window.AudioContext || window.webkitAudioContext;
const audioCtx = new AudioContext();

function setup() {
    var ids =
        [
            'click', 'draw', 'lose', 'move', 'win'
        ];
    var loaded = 0;
    ids.forEach(function (id) {
        var request = new XMLHttpRequest();
        request.open('GET', cross_asset_domain_ + 'wave/' + id + '.wav', cross_asset_async_);
        request.responseType = 'arraybuffer';
        request.onload = function () {
            var audioData = request.response;
            audioCtx.decodeAudioData(audioData, function (buffer) {
                audios_[id] = buffer;
                if (++loaded == ids.length) {
                    setTimeout(CallHandler, 0, 'body', 'setup', '');
                }
            });
        };
        request.send();
    });
}

function playAudio(id) {
    var source = audioCtx.createBufferSource();
    source.buffer = audios_[id];
    source.connect(audioCtx.destination);
    source.start(0);
}

function setAlter(state) {
    alter_ = state;
    document.getElementById('side-piece').setAttribute('src',
        alter_ ? 'pieces/bn.png' : 'pieces/wn.png');
}

function setRotate(state) {
    rotate_ = state;
    document.getElementById('side-direction').setAttribute('src',
        rotate_ ? 'sides/down.png' : 'sides/up.png');
}

function setThumb(state) {
    if (state) {
        document.body.style.setProperty('justify-content', 'end');
        document.getElementById('header').style.order = 1;
        document.getElementById('options').style.removeProperty('top');
        document.getElementById('options').style.bottom = '125%';
    } else {
        document.body.style.setProperty('justify-content', 'start');
        document.getElementById('header').style.order = 0;
        document.getElementById('options').style.top = '125%';
        document.getElementById('options').style.removeProperty('bottom');
    }
}

function createCell(index, x, y, shift) {
    var room = document.createElement('div');
    room.classList.add('room');
    room.onclick = function () {
        CallHandler("cell", "click", rotateIndex(index).toString());
    };
    var piece = document.createElement('div');
    piece.classList.add('piece');
    var text = document.createElement('div');
    text.classList.add('text');
    var cell = document.createElement('div');
    cell.classList.add('cell');
    cell.appendChild(room);
    cell.appendChild(piece);
    cell.appendChild(text);
    cell.style.setProperty('--x', x);
    cell.style.setProperty('--y', y);
    cell.style.setProperty('--shift', shift);
    document.getElementById('board').appendChild(cell);
}

function stop() {
    more();
    CallHandler("game", "stop", "");
}

function switchSides() {
    more();
    CallHandler("game", "switch", "");
}

function newGame() {
    more();
    CallHandler("game", "reset", "");
}

function gameOver(state) {
    var banner = document.getElementById("gameover");
    banner.setAttribute("data-show", state);
    switch (state) {
        case 0:
            banner.textContent = "";
            break;
        case 1:
            banner.textContent = "You Win!";
            break;
        case 2:
            banner.textContent = "Game Over!";
            break;
        case 3:
            banner.textContent = "Locked!";
            break;
    }
}

function more() {
    var options = document.getElementById("options");
    if (options.style.display == "block") {
        options.style.display = "none";
    }
    else {
        options.style.display = "block";
    }
}

function setPiece(index, type) {
    var cell = document.getElementById('board').children[rotateIndex(index)];
    var piece = cell.children[1];
    switch (type) {
        case -2:
            piece.setAttribute('data-color', alter_ ? 'bq' : 'wq');
            break;
        case -1:
            piece.setAttribute('data-color', alter_ ? 'bn' : 'wn');
            break;
        case 0:
            piece.removeAttribute('data-color');
            break;
        case 1:
            piece.setAttribute('data-color', alter_ ? 'wn' : 'bn');
            break;
        case 2:
            piece.setAttribute('data-color', alter_ ? 'wq' : 'bq');
            break;
    }
    var text = cell.children[2];
    text.innerText = '';
}

function setOrder(index, order) {
    var text = document.getElementById('board')
        .children[rotateIndex(index)].children[2];
    text.innerText = (text.innerText ?
        text.innerText + ' ' : '') + order;
}

function setGo(state) {
    var go = document.getElementById('go');
    switch (state) {
        case 1:
            go.innerText = "Go";
            break;
        case 2:
            go.innerText = "OK";
            break;
    }
    go.setAttribute('data-show', state);
}

function go() {
    CallHandler("game", "go", "");
}

function setMessage(state) {
    var message = document.getElementById('message');
    switch (state) {
        case 1:
            message.innerText = "Your Turn";
            break;
        case 2:
            message.innerText = "Wait";
            break;
    }
    message.setAttribute('data-show', state);
}

function rotateIndex(index) {
    if (rotate_) {
        return document.getElementById('board').children.length - 1 - index;
    }
    else {
        return index;
    }
}
