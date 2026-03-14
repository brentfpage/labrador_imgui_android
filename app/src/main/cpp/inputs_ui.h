#ifndef INPUTSUI_H
#define INPUTSUI_H
class inputsUI
{
    bool scope750 = false;
    bool changed = false;
public:
    void update_device_mode();
    bool logic_enable[2] = {0,0};
    bool scope_enable[2] = {true,false};
    void draw();
    bool changed_since_last();
    bool ch_enabled(int ch);
    enum Mode {Ch1Scope,ScopeLogic,ScopeScope,Ch1Logic,LogicLogic,None,Scope750,Multimeter};
    Mode mode = Mode::Ch1Scope;
    bool xy;
    bool logic_on();
    bool scopelogic_mode();
};

#endif // INPUTSUI_H
