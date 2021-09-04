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

function setSide(index, color)
{
    var cell = document.getElementById('board').children[index];
    if (color > 0)
    {
        cell.setAttribute('data-color', color);
    }
    else
    {
        cell.removeAttribute('data-color');
    }
    cell.removeAttribute('data-order');
}

function setOrder(index, order)
{
    var cell = document.getElementById('board').children[index];
    cell.setAttribute('data-order', order);
}

function cellClick(cell)
{
    CallHandler("cell", "click", cell.getAttribute('data-index'));
}
