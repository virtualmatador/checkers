var alter_ = false;
var rotate_ = false;

function setAlter(alter)
{
    alter_ = alter;
}

function setRotate(rotate)
{
    rotate_ = rotate;
}

function createCell(index, x, y, shift)
{
    var cell = document.createElement('div');
    cell.classList.add('cell');
    cell.onclick = function()
    {
        CallHandler("cell", "click", rotateIndex(index));
    };
    cell.style.setProperty('--x', x);
    cell.style.setProperty('--y', y);
    cell.style.setProperty('--shift', shift);
    document.getElementById('board').appendChild(cell);
}

function newGame()
{
    CallHandler("game", "reset", "");
}

function stop()
{
    CallHandler("game", "stop", "");
}

function giveUp()
{
    CallHandler("game", "giveup", "");
}

function switchSides()
{
    CallHandler("game", "switch", "");
}

function gameOver(state)
{
    var banner = document.getElementById("gameover");
    banner.setAttribute("data-show", state);
    switch(state)
    {
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
            banner.textContent = "Game locked!";
        break;
    }
}

function more(hide)
{
    var options = document.getElementById("options");
    if (hide || options.style.display == "block")
    {
        options.style.display = "none";
    }
    else
    {
        options.style.display = "block";
    }
}

window.onclick = function(event)
{
    if (event.target != document.getElementById("more"))
    {
        more(true);
    }
}

function setPiece(index, piece)
{
    var cell = document.getElementById('board').children[rotateIndex(index)];
    switch (piece)
    {
    case -2:
        cell.setAttribute('data-color', alter_ ? 'bq' : 'wq');
        break;
    case -1:
        cell.setAttribute('data-color', alter_ ? 'bn' : 'wn');
        break;
    case 0:
        cell.removeAttribute('data-color');
        break;
    case 1:
        cell.setAttribute('data-color', alter_ ? 'wn' : 'bn');
        break;
    case 2:
        cell.setAttribute('data-color', alter_ ? 'wq' : 'bq');
        break;
    }
    cell.removeAttribute('data-order');
}

function setOrder(index, order)
{
    var cell = document.getElementById('board').children[rotateIndex(index)];
    cell.setAttribute('data-order', (cell.getAttribute('data-order') ?
        cell.getAttribute('data-order') + ' ' : '') + order);
}

function setGo(state)
{
    var go = document.getElementById('go');
    switch (state)
    {
    case 1:
        go.innerText = "Go";
    break;
    case 2:
        go.innerText = "OK";
    break;
    }
    go.setAttribute('data-show', state);
}

function go()
{
    CallHandler("game", "go", "");
}

function setMessage(state)
{
    var message = document.getElementById('message');
    switch (state)
    {
    case 1:
        message.innerText = "Your Turn";
    break;
    case 2:
        message.innerText = "Wait";
    break;
    }
    message.setAttribute('data-show', state);
}

function rotateIndex(index)
{
    if (rotate_)
    {
        return document.getElementById('board').children.length - 1 - index;
    }
    else
    {
        return index;
    }
}
