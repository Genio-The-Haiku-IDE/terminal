/*
 * Copyright 2025, Andrea Anzani <andrea.anzani@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "GenioTermView.h"
#include "TermView.h"
#include <ControlLook.h>
#include <cstdio>
#include <Messenger.h>

const static int32 kTermViewOffset = 3;

class GenioTermViewContainerView : public BView {
public:
	GenioTermViewContainerView(BView* termView)
		:
		BView(BRect(), "term view container", B_FOLLOW_ALL, 0),
		fTermView(termView)
	{
		termView->MoveTo(kTermViewOffset, kTermViewOffset);
		BRect frame(termView->Frame());
		ResizeTo(frame.right + kTermViewOffset, frame.bottom + kTermViewOffset);
		AddChild(termView);
		termView->SetResizingMode(B_FOLLOW_ALL);
	}

	virtual void GetPreferredSize(float* _width, float* _height)
	{
		float width, height;
		fTermView->GetPreferredSize(&width, &height);
		*_width = width + 2 * kTermViewOffset;
		*_height = height + 2 * kTermViewOffset;
	}

	void AttachedToWindow()
	{
		SetViewColor(rgb_color{255,0,0});
	}

private:
	BView*	fTermView;
};



class GenioListener : public TermView::Listener {
public:
								GenioListener(BMessenger& msg): fMessenger(msg)
									{};

	virtual						~GenioListener(){};

	// all hooks called in the window thread
	virtual	void				NotifyTermViewQuit(TermView* view,
									int32 reason) {
										printf("NotifyTermViewQuit OK [%s]\n", fMessenger.Target(nullptr)->Name());
									BMessage msg('NOTM');
									printf("SEND %s\n", strerror(fMessenger.SendMessage(&msg)));
									}
	virtual	void				SetTermViewTitle(TermView* view,
									const char* title)
									{
									printf("|| SetTermViewTitle %s\n", title);
									}
	virtual	void				PreviousTermView(TermView* view)
	{
		printf("|| PreviousTermView\n");
	}
	virtual	void				NextTermView(TermView* view)
	{
		printf("|| NextTermView\n");
	}

	BMessenger fMessenger;
};



/*static*/
BArchivable*
GenioTermView::Instantiate(BMessage* data)
{
	printf("data "); data->PrintToStream();
	if (validate_instantiation(data, "GenioTermView")) {
		TermView *view = new (std::nothrow) TermView(data);
		BMessenger messenger;
		if (data->FindMessenger("listener", &messenger) == B_OK) {
			printf("LISTENER OK\n");
			view->SetListener(new GenioListener(messenger));
		}
		printf("view "); view->Bounds().PrintToStream();
		GenioTermViewContainerView* containerView = new (std::nothrow) GenioTermViewContainerView(view);
		printf("containerView "); view->Bounds().PrintToStream();
		BScrollView* scrollView = new (std::nothrow) TermScrollView("scrollView",
			containerView, view, true);
		printf("scrollView "); view->Bounds().PrintToStream();

		scrollView->ScrollBar(B_VERTICAL)
				->ResizeBy(0, -(be_control_look->GetScrollBarWidth(B_VERTICAL) - 1));

		printf("Qui quo qua\n");
		return scrollView;
	}

	return NULL;
}
