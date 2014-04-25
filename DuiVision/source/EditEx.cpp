#include "StdAfx.h"
#include "EditEx.h"

CEditEx::CEditEx(HWND hWnd, CDuiObject* pDuiObject)
						   : CControlBaseFont(hWnd, pDuiObject)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bDown = false;
	m_bDownTemp = false;
	m_buttonState = enBSNormal;
	m_EditState = enBSNormal;
	m_pLeftImage = NULL;
	m_pSmallImage = NULL;
	m_pEdit = NULL;
	m_bPassWord = FALSE;
	m_strTitle = _T("");

	SetRect(CRect(0,0,0,0));
	SetBitmapCount(4);

	m_sizeLeftImage.SetSize(0,0);
	m_sizeSmallImage.SetSize(0,0);

	m_bPassWord = false;
	m_bMultiLine = false;
	m_bAutoHScroll = false;
	m_bAutoVScroll = false;
	m_bNumber = false;
	m_bReadOnly = false;
	m_nMaxChar = -1;

	m_bIsSmallButton = FALSE;

	VERIFY(m_fontTemp.CreateFont(
		18,							// 字体的高度  
		0,							// 字体的宽度  
		0,							// 字体显示的角度
		0,							// 字体的角度
		FW_DONTCARE,				// 字体的磅数
		FALSE,						// 斜体字体
		FALSE,						// 带下划线的字体
		0,							// 带删除线的字体
		GB2312_CHARSET,				// 所需的字符集
		OUT_DEFAULT_PRECIS,			// 输出的精度
		CLIP_DEFAULT_PRECIS,		// 裁减的精度
		DEFAULT_QUALITY,			// 逻辑字体与输出设备的实际
		DEFAULT_PITCH | FF_SWISS,	// 字体间距和字体集
		DuiSystem::GetDefaultFont()));					// 字体名称
}

CEditEx::CEditEx(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle/* = ""*/, BOOL bPassWord/* = FALSE*/, BOOL bIsVisible/* = TRUE*/, 
						   BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
						   : CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bDown = false;
	m_bDownTemp = false;
	m_buttonState = enBSNormal;
	m_EditState = enBSNormal;
	m_pLeftImage = NULL;
	m_pSmallImage = NULL;
	m_pEdit = NULL;
	m_bPassWord = bPassWord;
	m_strTitle = strTitle;

	m_sizeLeftImage.SetSize(0,0);
	m_sizeSmallImage.SetSize(0,0);

	SetRect(rc);
	SetBitmapCount(4);

	m_bPassWord = false;
	m_bMultiLine = false;
	m_bAutoHScroll = false;
	m_bAutoVScroll = false;
	m_bNumber = false;
	m_bReadOnly = false;
	m_nMaxChar = -1;

	m_bIsSmallButton = FALSE;

	VERIFY(m_fontTemp.CreateFont(
		18,							// 字体的高度  
		0,							// 字体的宽度  
		0,							// 字体显示的角度
		0,							// 字体的角度
		FW_DONTCARE,				// 字体的磅数
		FALSE,						// 斜体字体
		FALSE,						// 带下划线的字体
		0,							// 带删除线的字体
		GB2312_CHARSET,				// 所需的字符集
		OUT_DEFAULT_PRECIS,			// 输出的精度
		CLIP_DEFAULT_PRECIS,		// 裁减的精度
		DEFAULT_QUALITY,			// 逻辑字体与输出设备的实际
		DEFAULT_PITCH | FF_SWISS,	// 字体间距和字体集
		DuiSystem::GetDefaultFont()));					// 字体名称
}

CEditEx::~CEditEx(void)
{
	// 删除Windows控件
	HideEdit();

	if (m_fontTemp.m_hObject)
	{
		m_fontTemp.DeleteObject();
	}

	if(m_pLeftImage != NULL)
	{
		delete m_pLeftImage;
		m_pLeftImage = NULL;
	}

	if(m_pSmallImage != NULL)
	{
		delete m_pSmallImage;
		m_pSmallImage = NULL;
	}
}

bool CEditEx::SetLeftBitmap(UINT nResourceID, CString strType)
{
	if(ImageFromIDResource(nResourceID, strType, m_pLeftImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight()) >= 3)
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetWidth() / 4, m_pLeftImage->GetHeight());
			m_nLeftImageCount = 4;
		}else
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetHeight(), m_pLeftImage->GetHeight());
			m_nLeftImageCount = m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight();
		}
		return true;
	}
	return false;
}

bool CEditEx::SetLeftBitmap(CString strImage)
{
	m_pLeftImage = Image::FromFile(strImage, TRUE);

	if(m_pLeftImage->GetLastStatus() == Ok)
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight()) >= 3)
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetWidth() / 4, m_pLeftImage->GetHeight());
			m_nLeftImageCount = 4;
		}else
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetHeight(), m_pLeftImage->GetHeight());
			m_nLeftImageCount = m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight();
		}
		return true;
	}
	return false;
}

// 从XML设置图片信息属性
HRESULT CEditEx::OnAttributeLeftImage(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// 加载图片文件
	{
		CString strImgFile = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			strImgFile = CA2T(strSkin, CP_UTF8);
		}
		if(!SetLeftBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetLeftBitmap(nResourceID, TEXT("PNG")))
		{
			if(!SetLeftBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

bool CEditEx::SetSmallBitmap(UINT nResourceID, CString strType)
{
	if(ImageFromIDResource(nResourceID, strType, m_pSmallImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight()) >= 3)
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetWidth() / 4, m_pSmallImage->GetHeight());
			m_nSmallImageCount = 4;
			m_bIsSmallButton = TRUE;
		}else
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetHeight(), m_pSmallImage->GetHeight());
			m_nSmallImageCount = m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight();
			m_bIsSmallButton = FALSE;
		}
		return true;
	}
	return false;
}

bool CEditEx::SetSmallBitmap(CString strImage)
{
	m_pSmallImage = Image::FromFile(strImage, TRUE);

	if(m_pSmallImage->GetLastStatus() == Ok)
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight()) >= 3)
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetWidth() / 4, m_pSmallImage->GetHeight());
			m_nSmallImageCount = 4;
			m_bIsSmallButton = TRUE;
		}else
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetHeight(), m_pSmallImage->GetHeight());
			m_nSmallImageCount = m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight();
			m_bIsSmallButton = FALSE;
		}
		return true;
	}
	return false;
}

// 从XML设置图片信息属性
HRESULT CEditEx::OnAttributeSmallImage(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// 加载图片文件
	{
		CString strImgFile = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			strImgFile = CA2T(strSkin, CP_UTF8);
		}
		if(!SetSmallBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetSmallBitmap(nResourceID, TEXT("PNG")))
		{
			if(!SetSmallBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

void  CEditEx::SetControlRect(CRect rc) 
{
	m_rc = rc;
	m_rcText = m_rc;
	m_rcText.top += 4;
	m_rcText.left += (6 + m_sizeLeftImage.cx);
	m_rcText.bottom -= 4;
	m_rcText.right -= (3 + m_sizeSmallImage.cx);
}

BOOL CEditEx::IsDraw(CPoint point)
{
 	if(m_buttonState == enBSDown)
 	{
 		return false;
 	}
	CRect  rc;
	rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
	rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
	rc.right = rc.left + m_sizeSmallImage.cx;
	rc.bottom = rc.top + m_sizeSmallImage.cy;

	if(rc.PtInRect(point))
	{
		if(m_buttonState == enBSNormal)
			return true;
	}
	else
	{
		if(m_buttonState != enBSNormal)
			return true;
	}

	return false;
}

// 设置控件中的Windows原生控件是否可见的状态
void CEditEx::SetControlWndVisible(BOOL bIsVisible)
{
	if(bIsVisible)
	{
		ShowEdit();
	}else
	{
		HideEdit();
	}
}

// 设置控件的禁用状态
void CEditEx::SetControlDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		m_bIsDisable = bIsDisable;
		if(bIsDisable)
		{
			m_EditState = enBSDisable;
			m_buttonState = enBSDisable;
			HideEdit();
		}
		else
		{
			if(m_bDown)
			{
				m_EditState = enBSDown;
				m_buttonState = enBSDown;
			}
			else
			{
				m_EditState = enBSNormal;
				m_buttonState = enBSNormal;
			}
		}
		::InvalidateRect(m_hWnd, &m_rc, true);
	}
}

// 设置控件title
void CEditEx::SetControlTitle(CString strTitle)
{
	__super::SetControlTitle(strTitle);

	// 如果edit控件已经激活,则需要设置edit控件的内容
	if(m_pEdit)
	{
		m_pEdit->SetWindowText(m_strTitle);	
		m_pEdit->SetSel(m_strTitle.GetLength(), -1);
	}
}

// 设置控件的焦点
BOOL CEditEx::SetControlFocus(BOOL bFocus)
{
	__super::SetControlFocus(bFocus);

	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;

	if(!bFocus)
	{
		m_bDown = false;
		m_buttonState = enBSNormal;
		m_EditState = enBSNormal;
		HideEdit();
	}else
	{
		m_bDown = true;
		m_buttonState = enBSDown;
		m_EditState = enBSDown;
		ShowEdit();
	}

	bool bIsDraw = buttonState != m_buttonState || editState != m_EditState;
	if(bIsDraw)
	{
		::InvalidateRect(m_hWnd, &m_rc, true);
	}

	return bIsDraw;
}

BOOL CEditEx::OnControlMouseMove(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_EditState = enBSHover;
			if(m_buttonState != enBSDown)
			{
				CRect  rc;
				rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
				rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
				rc.right = rc.left + m_sizeSmallImage.cx;
				rc.bottom = rc.top + m_sizeSmallImage.cy;

				if(rc.PtInRect(point))
				{
					m_buttonState = enBSHover;
				}
				else
				{
					//ShowEdit();
					m_buttonState = enBSNormal;
				}
			}
		}
		else
		{
			if(m_buttonState != enBSDown)
			{
				m_buttonState = enBSNormal;
				m_EditState = enBSNormal;
			}
		}
	}
	
	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}

	return buttonState != m_buttonState || editState != m_EditState;
}

BOOL CEditEx::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;	
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			CRect  rc;
			rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
			rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
			rc.right = rc.left + m_sizeSmallImage.cx;
			rc.bottom = rc.top + m_sizeSmallImage.cy;

			if(rc.PtInRect(point))	// 点击的是编辑框右部的小按钮图片
			{
				if(m_bIsSmallButton)	// 小图片作为按钮才进行处理
				{
					m_bDown = !m_bDown;
					m_bDownTemp = true;
					if(m_bDown)
					{
						m_buttonState = enBSDown;
					}
					else
					{				
						m_buttonState = enBSHover;
					}				
					SendMessage(m_uID, BUTTOM, BUTTOM_DOWN);
					HideEdit();
				}
			}
			else
			{
				if(m_bDown)
				{
					m_bDown = false;
					m_buttonState = enBSHover;
				}
				ShowEdit();
				
				SendMessage(m_uID, EDIT, BUTTOM_DOWN);
			}		
		}
		else
		{
			m_buttonState = enBSNormal;
			m_EditState = enBSNormal;
		}
	}

	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}
	
	return buttonState != m_buttonState || editState != m_EditState;
}

BOOL CEditEx::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_EditState = enBSHover;

			CRect  rc;
			rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
			rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
			rc.right = rc.left + m_sizeSmallImage.cx;
			rc.bottom = rc.top + m_sizeSmallImage.cy;

			if(rc.PtInRect(point))
			{
				if(m_bIsSmallButton)	// 小图片作为按钮才进行处理
				{
					if(m_bDown)
					{
						m_buttonState = enBSDown;
					}
					else
					{
						m_buttonState = enBSHover;
					}	
					SendMessage(m_uID, BUTTOM, BUTTOM_UP);
				}
			}
			else
			{
				if(m_bDown)
				{
					m_buttonState = enBSDown;
				}
				else
				{
					m_buttonState = enBSNormal;
				}	
				SendMessage(m_uID, EDIT, BUTTOM_UP);
			}			
		}
		else
		{
			if(!m_bDown)
			{
				m_buttonState = enBSNormal;
				m_EditState = enBSNormal;
			}
		}
	}
	
	m_bDownTemp = false;

	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}

	return buttonState != m_buttonState || editState != m_EditState;
}

void CEditEx::DrawControl(CDC &dc, CRect rcUpdate)
{
	Graphics graphics(dc);

	DrawImageFrame(graphics, m_pImage, m_rc, m_EditState * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 4);

	if(m_pLeftImage)
	{
		CRect  rc;
		rc.left = m_rc.left + 2;
		rc.top = m_rc.top + (m_rc.Height() - m_sizeLeftImage.cy) / 2;
		rc.right = rc.left + m_sizeLeftImage.cx;
		rc.bottom = rc.top + m_sizeLeftImage.cy;
		
		if(m_nLeftImageCount > m_buttonState)
		{
			graphics.DrawImage(m_pLeftImage, RectF(rc.left , rc.top, rc.Width(), rc.Height()),
				m_buttonState * m_sizeLeftImage.cx, 0, m_sizeLeftImage.cx, m_sizeLeftImage.cy, UnitPixel);
		}else
		{
			graphics.DrawImage(m_pLeftImage, RectF(rc.left , rc.top, rc.Width(), rc.Height()),
				0, 0, m_sizeLeftImage.cx, m_sizeLeftImage.cy, UnitPixel);
		}
	}

	if(m_pSmallImage)
	{
		CRect  rc;
		rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
		rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
		rc.right = rc.left + m_sizeSmallImage.cx;
		rc.bottom = rc.top + m_sizeSmallImage.cy;
		
		if(m_nSmallImageCount > m_buttonState)
		{
			graphics.DrawImage(m_pSmallImage, RectF(rc.left , rc.top, rc.Width(), rc.Height()),
				m_buttonState * m_sizeSmallImage.cx, 0, m_sizeSmallImage.cx, m_sizeSmallImage.cy, UnitPixel);
		}else
		{
			graphics.DrawImage(m_pSmallImage, RectF(rc.left , rc.top, rc.Width(), rc.Height()),
				0, 0, m_sizeSmallImage.cx, m_sizeSmallImage.cy, UnitPixel);
		}
	}

	FontFamily fontFamily(m_strFont.AllocSysString());
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
	SolidBrush solidBrush(m_clrText);
	SolidBrush solidBrushTip(m_clrTooltip);
	graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);		// 水平方向左对齐
	if(!m_bMultiLine)
	{
		// 单行文字
		strFormat.SetLineAlignment(StringAlignmentCenter);	// 垂直方向中间对齐
		strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
	}else
	{
		strFormat.SetLineAlignment(StringAlignmentNear);	// 垂直方向上对齐
	}

	RectF rect(m_rcText.left, m_rcText.top+2, m_rcText.Width(), m_rcText.Height()-2);

	if(!m_strTitle.IsEmpty())
	{
		// 文字非空
		CString strTitle = m_strTitle;
		if(m_bPassWord)
		{
			int nlen = strTitle.GetLength();
			strTitle = "";
			for(int i = 0; i < nlen; i++)
			{
				strTitle += '*';
			}
		}

		graphics.DrawString(strTitle.AllocSysString(), (INT)wcslen(strTitle.AllocSysString()), &font, 
					rect, &strFormat, &solidBrush);
	}else
	if(!m_strTooltip.IsEmpty())
	{
		// 如果没有文字,但设置了tooltip,则显示tooltip
		graphics.DrawString(m_strTooltip.AllocSysString(), (INT)wcslen(m_strTooltip.AllocSysString()), &font, 
					rect, &strFormat, &solidBrushTip);
	}
}

BOOL CEditEx::GetLButtonDown()
{
	return m_buttonState == enBSDown;
}

BOOL CEditEx::GetLButtonUp()
{
	return FALSE;
}

CString CEditEx::GetEditText()
{
	if(m_pEdit && ::IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->GetWindowText(m_strTitle);
	}
	return m_strTitle;
}

void CEditEx::ShowEdit()
{
	// 如果是只读属性，则不用创建edit控件
	if(m_bReadOnly || m_bIsDisable)
	{
		return;
	}

 	if(NULL == m_pEdit)
 	{
		TestMainThread();	// 测试是否在主线程

		CRect rc;
		rc = m_rcText;
		//rc.left--;
		if(!m_bMultiLine)
		{
			// 仅针对单行编辑框调整位置
			rc.top += (m_rc.Height() - 18 - 6) / 2;	// 调整windows控件激活时候的显示位置,和非激活状态下更接近一些
		}else
		{
			rc.top += 2;
		}
  		m_pEdit = new CEdit;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
		if(m_bMultiLine)
		{
			dwStyle |= ES_MULTILINE;
		}
		if(m_bAutoHScroll)
		{
			dwStyle |= ES_AUTOHSCROLL;
		}
		if(m_bAutoVScroll)
		{
			dwStyle |= ES_AUTOVSCROLL;
		}
		if(m_bNumber)
		{
			dwStyle |= ES_NUMBER;
		}
		if(m_bReadOnly)
		{
			dwStyle |= ES_READONLY;
		}
  		m_pEdit->Create(dwStyle, rc, CWnd::FromHandle(m_hWnd), 1111); 
  		m_pEdit->SetFont(&m_fontTemp);
		m_pEdit->SetWindowText(m_strTitle);	
		if(m_bPassWord)
		{
			m_pEdit->SetPasswordChar('*');
		}
		if(m_nMaxChar > 0)
		{
			m_pEdit->LimitText(m_nMaxChar);
		}
		m_pEdit->SetSel(m_strTitle.GetLength(), -1);
		m_pEdit->SetFocus();
 	}
}

void CEditEx::HideEdit()
{
	if(m_pEdit)
	{
		TestMainThread();	// 测试是否在主线程

		// 调用Edit的Windows控件必须先判断窗口是否有效
		if(::IsWindow(m_pEdit->GetSafeHwnd()))
		{
			// 获取编辑框的内容保存在控件的变量中
			m_pEdit->GetWindowText(m_strTitle);
		}
		delete m_pEdit;
		m_pEdit = NULL;
	}
}

// 消息处理
LRESULT CEditEx::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::OnMessage(uID, uMsg, wParam, lParam);
}