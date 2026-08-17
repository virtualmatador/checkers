(function() {
    "use strict";

    var preferences = ["system", "light", "dark"];
    var systemPreference = "system";
    var darkScheme = window.matchMedia("(prefers-color-scheme: dark)");
    var preference = null;

    function normalizePreference(value) {
        if (preferences.indexOf(value) >= 0) {
            return value;
        }
        var index = Number(value);
        if (Number.isInteger(index) && preferences[index]) {
            return preferences[index];
        }
        return systemPreference;
    }

    function resolvedTheme() {
        if (preference !== systemPreference) {
            return preference;
        }
        return darkScheme.matches ? "dark" : "light";
    }

    function applyTheme() {
        var theme = resolvedTheme();
        document.documentElement.setAttribute("data-theme", theme);
        var colorScheme = document.querySelector('meta[name="color-scheme"]');
        if (colorScheme) {
            colorScheme.setAttribute("content", theme);
        }
        var themeColor = document.querySelector('meta[name="theme-color"]');
        if (themeColor) {
            themeColor.setAttribute("content",
                theme === "dark" ? "#020617" : "#ffffff");
        }
    }

    window.setThemePreference = function(value) {
        preference = normalizePreference(value);
        applyTheme();
        var select = document.getElementById("theme");
        if (select) {
            select.value = preferences.indexOf(preference).toString();
        }
    };

    function systemThemeChanged() {
        if (preference === systemPreference) {
            applyTheme();
        }
    }

    if (darkScheme.addEventListener) {
        darkScheme.addEventListener("change", systemThemeChanged);
    }
    else {
        darkScheme.addListener(systemThemeChanged);
    }
})();
