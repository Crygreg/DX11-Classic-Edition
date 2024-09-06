#pragma once
#include "BaseWidget.h"

enum EWTR_Selection {
    WTR_None,
    WTR_TransX,
    WTR_TransY,
    WTR_TransZ
};

class Widget_TransRot : public BaseWidget {
public:
    Widget_TransRot( WidgetContainer* container );
    ~Widget_TransRot();

    /** Renders the widget */
    virtual void RenderWidget();

    /** Called when the owning window got a message */
    virtual void OnWindowMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

    /** Called when a mousebutton was clicked */
    virtual void OnMButtonClick( int button );

    /** Returns whether this widget is active or not */
    virtual bool IsActive() const override;

    /** Called when an object was added to the selection */
    virtual void OnSelectionAdded( zCVob* vob );

protected:
    /** Applys the transforms to all parts of the widget */
    void ApplyTransforms();

    /** Checks if one of the widget parts are selected */
    void DoHoverTest( HWND hw );

    EditorLinePrimitive* TransLines[3];
    EditorLinePrimitive* Arrows[3];
    EditorLinePrimitive* Circles[3];
    EditorLinePrimitive* RotBackpanel;

    EWTR_Selection ActiveSelection;
    EWTR_Selection ActiveWidget;

    POINT LastMousePosition;
};
