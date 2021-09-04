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
        cell.setAttribute('data-color', alter_ ? 'wq' : 'bq');
        break;
    case -1:
        cell.setAttribute('data-color', alter_ ? 'wn' : 'bn');
        break;
    case 0:
        cell.removeAttribute('data-color');
        break;
    case 1:
        cell.setAttribute('data-color', alter_ ? 'bn' : 'wn');
        break;
    case 2:
        cell.setAttribute('data-color', alter_ ? 'bq' : 'wq');
        break;
    }
    cell.removeAttribute('data-order');
}

function setOrder(index, order)
{
    var cell = document.getElementById('board').children[rotateIndex(index)];
    cell.setAttribute('data-order', order);
}

function cellClick(cell)
{
    CallHandler("cell", "click", rotateIndex(cell.getAttribute('data-index')));
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
