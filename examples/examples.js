function loadApp(ps, app, res) {
    var component = Qt.createComponent(app);
    if (component.status === Component.Ready) {
        var obj = component.createObject(ps);
        obj.rootPath = res;
        ps.push(obj);
    } else {
        console.log("ERROR: Unable to load app" + app);
    }
}
