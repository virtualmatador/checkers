var game_over_ = 0;
var pending_action_ = "";
var previous_focus_ = null;
var action_submitted_ = false;

function play() {
    CallHandler("play", "click", "");
}

function reset() {
    if (game_over_ !== 0) {
        submitAction("reset");
    }
    else {
        showConfirmation("Reset the game and all options?", "reset");
    }
}

function switchSides() {
    showConfirmation("Switch sides?", "switch");
}

function setGameOver(state) {
    game_over_ = state;
    document.getElementById("switch-sides").disabled = false;
    document.getElementById("reset").disabled = false;
}

function showConfirmation(message, action) {
    if (action_submitted_) {
        return;
    }
    pending_action_ = action;
    previous_focus_ = document.activeElement;
    document.getElementById("confirmation-title").textContent = "Confirm";
    document.getElementById("confirmation-message").textContent = message;
    document.getElementById("confirmation-panel").removeAttribute("aria-busy");
    document.getElementById("confirmation-cancel").disabled = false;
    document.getElementById("confirmation-accept").disabled = false;
    document.getElementById("confirmation").hidden = false;
    document.getElementById("confirmation-cancel").focus();
}

function closeConfirmation(restoreFocus) {
    document.getElementById("confirmation").hidden = true;
    pending_action_ = "";
    if (restoreFocus && previous_focus_ && previous_focus_.focus) {
        previous_focus_.focus();
    }
    previous_focus_ = null;
}

function cancelConfirmation() {
    if (action_submitted_) {
        return;
    }
    closeConfirmation(true);
}

function acceptConfirmation() {
    if (action_submitted_) {
        return;
    }
    var action = pending_action_;
    closeConfirmation(false);
    submitAction(action);
}

function cancelConfirmationOnBackdrop(event) {
    if (event.target === event.currentTarget) {
        cancelConfirmation();
    }
}

function confirmationKeyDown(event) {
    if (action_submitted_) {
        event.preventDefault();
        return;
    }
    if (event.key === "Escape") {
        event.preventDefault();
        cancelConfirmation();
    }
    else if (event.key === "Tab") {
        var cancel = document.getElementById("confirmation-cancel");
        var accept = document.getElementById("confirmation-accept");
        if (event.shiftKey && document.activeElement === cancel) {
            event.preventDefault();
            accept.focus();
        }
        else if (!event.shiftKey && document.activeElement === accept) {
            event.preventDefault();
            cancel.focus();
        }
    }
}

function submitAction(action) {
    if (action_submitted_ || (action !== "reset" && action !== "switch")) {
        return;
    }
    action_submitted_ = true;
    document.getElementById("switch-sides").disabled = true;
    document.getElementById("reset").disabled = true;
    document.getElementById("confirmation-title").textContent = "Working";
    document.getElementById("confirmation-message").textContent = "Please wait…";
    document.getElementById("confirmation-panel").setAttribute("aria-busy", "true");
    document.getElementById("confirmation-cancel").disabled = true;
    document.getElementById("confirmation-accept").disabled = true;
    document.getElementById("confirmation").hidden = false;
    document.getElementById("confirmation-panel").focus();
    CallHandler(action, "click", "");
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
