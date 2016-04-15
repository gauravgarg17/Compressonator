//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// \file cpTreeWidget.cpp
/// \version 2.21
//
//=====================================================================

#include "cpTreeWidget.h"
#include "PluginManager.h"

extern PluginManager g_pluginManager;


cpTreeWidget::cpTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    m_currentItem = NULL;
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selChanged()));
}

cpTreeWidget::~cpTreeWidget()
{
}

void cpTreeWidget::selChanged()
{
}

void cpTreeWidget::mousePressEvent(QMouseEvent  *event)
{
    QTreeWidget::mousePressEvent(event);

    if (currentItem())
    {
        m_currentItem = currentItem();
    }

    emit event_mousePress(event);
}

void cpTreeWidget::keyPressEvent(QKeyEvent* event)
{ 
    //bool SHIFT_Key = event->modifiers() & Qt::ShiftModifier;
    bool CTRL_key  = event->modifiers() & Qt::ControlModifier;

    QTreeWidget::keyPressEvent(event);    
    switch (event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        if (currentItem())
        {
            emit QTreeWidget::itemDoubleClicked(currentItem(), 0);
        }
        break;
    }
    case Qt::Key_Space:
    case Qt::Key_Select:
            {
                if (currentItem())
                {
                    emit QTreeWidget::itemClicked(currentItem(), 0);
                }
                break;
            }
    case Qt::Key_A:
        {
            if (CTRL_key)
            {
                qDebug() << "CTRL-A";
            }
            break;
        }
    } 

    if (currentItem())
    {
        m_currentItem = currentItem();
    }

    emit event_keyPress(event);
}


bool cpTreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
{
    Q_UNUSED(action);
    Q_UNUSED(index);
    Q_UNUSED(parent);

    if (!data) return false;

    QList<QUrl> urlList;
    urlList = data->urls(); // retrieve list of urls
    QString filePathName;

    foreach(QUrl url, urlList) // iterate over list
    {
        filePathName = url.toLocalFile();

        // Get file Extension and check if it can be loaded by our AMD plugin
        QFileInfo fi(filePathName.toUpper());
        QString name = fi.fileName();
        QStringList list1 = name.split(".");
        QString PlugInType = list1[list1.size() - 1];
        QByteArray ba = PlugInType.toLatin1();
        const char *Ext = ba.data();
        
        qApp->setOverrideCursor(Qt::WaitCursor);

        QImageReader imageFormat(filePathName);

        if (imageFormat.canRead())
        {
            emit DroppedImageItem(filePathName, index);
        }
        else
        // check if its an AMD supported image item
        if (g_pluginManager.PluginSupported("IMAGE", (char *)Ext))
        {
            emit DroppedImageItem(filePathName, index);
        }

        qApp->restoreOverrideCursor();

    }

    return true;    
}


QStringList cpTreeWidget::mimeTypes () const 
{
    QStringList qstrList;
    // list of accepted mime types for drop
    qstrList.append("text/uri-list");
    return qstrList;
}


Qt::DropActions cpTreeWidget::supportedDropActions () const
{
    // returns what actions are supported when dropping
    return Qt::CopyAction | Qt::MoveAction;
}


