
// MFC_tutorial3View.h : interface of the CMFCtutorial3View class
//

#pragma once


class CMFCtutorial3View : public CView
{
protected: // create from serialization only
	CMFCtutorial3View() noexcept;
	DECLARE_DYNCREATE(CMFCtutorial3View)

// Attributes
public:
	CMFCtutorial3Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMFCtutorial3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnToolsRotate45();
	afx_msg void OnToolsRotate();
	afx_msg void OnToolsRotate90();
	afx_msg void OnToolsRotate32774();
	afx_msg void OnToolsSharpen();
	afx_msg void OnMenu();
	afx_msg void OnToolsZoomout();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo32778();
	afx_msg void OnToolsEdgedetection();
	afx_msg void OnToolsFliphorizontally();
	afx_msg void OnToolsFlipvertically();
	afx_msg void OnToolsLaplacian();
};

#ifndef _DEBUG  // debug version in MFC_tutorial3View.cpp
inline CMFCtutorial3Doc* CMFCtutorial3View::GetDocument() const
   { return reinterpret_cast<CMFCtutorial3Doc*>(m_pDocument); }
#endif

