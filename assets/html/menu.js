function play()
{
    CallHandler("play", "click", "");
}

function reset()
{
    CallHandler("reset", "click", "");
}

function setAlter(alter)
{
    document.getElementById("alter").checked = alter;
}

function setRotate(rotate)
{
    document.getElementById("rotate").checked = rotate;
}

function setSound(state)
{
    document.getElementById("sound").checked = state;
}

function alter()
{
    CallHandler("alter", "click", document.getElementById("alter").checked.toString());
}

function rotate()
{
    CallHandler("rotate", "click", document.getElementById("rotate").checked.toString());
}

function sound()
{
    CallHandler("sound", "click", document.getElementById("sound").checked.toString());
}
