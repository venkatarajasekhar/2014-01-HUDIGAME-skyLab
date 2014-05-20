﻿using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using System.Threading.Tasks;

namespace GameTool
{
    public partial class IndependentGameTool : Form
    {
        Class.Renderer m_Renderer = new Class.Renderer();
        Class.JSONInOut m_JsonManager = new Class.JSONInOut();

        public IndependentGameTool()
        {
            InitializeComponent();

            this.ObjectView.MouseWheel += new System.Windows.Forms.MouseEventHandler(ObjectCameraZoomInOut);
        }

        private void ISSRenderStart(object sender, EventArgs e)
        {
            m_Renderer.CreateDevice(this.ObjectView);

            Render();
        }

        private async void Render()
        {
            // 무한 루프
            while(true)
            {
                m_Renderer.Render();
                await Task.Delay(5);
            }
        }

        private void ISSPartRenderClick(object sender, EventArgs e)
        {
            this.Activate();
            this.ObjectView.Focus();
        }

        private void ObjectCameraZoomInOut(object sender, MouseEventArgs e)
        {
            // 아래로 휠
            if ( e.Delta > 0 )
            {
                m_Renderer.ZoomInOutCameraPosition(-1);
            }
            else
            {
                m_Renderer.ZoomInOutCameraPosition(1);
            }
        }

        private void ObjViewMouseLeave(object sender, EventArgs e)
        {
            this.Focus();
        }

        private void SearchJsonFile(object sender, EventArgs e)
        {
            m_JsonManager.SearchJsonFiles(this.JsonFileList);
        }

        private void LoadJsonFile(object sender, EventArgs e)
        {
            m_JsonManager.LoadJsonFile(JsonFileList, JsonVariables, RenderStartBtn);
        }

        private void TreeViewJsonDataSelected(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNode tn = e.Node;
            if (null != tn)
            {
                string val = m_JsonManager.SplitJSONValueFromKeyValue(tn.Text);
                string key = m_JsonManager.SplitJSONKeyFromKeyValue(tn.Text);
                JSONKeyLabel.Text = key;
                JSONVarBar.Text = val;
            }
        }

        private void JsonModifyDataBtn(object sender, EventArgs e)
        {
            TreeNode tn = JsonVariables.SelectedNode;
            if (null != tn)
            {
                // 바뀐 JSON 데이터를 반영해준다
                ChangeJsonData(tn, JSONVarBar.Text);
                // Tree에 바뀐 값을 집어넣는다.
                if (tn.Text.Contains(":")) // Key : Value 형태
                {
                    tn.Text = JSONKeyLabel.Text + " : " + JSONVarBar.Text;
                }
                else // Value 형태
                {
                    tn.Text = JSONVarBar.Text;
                }
            }
        }

        private void ChangeJsonData(TreeNode node, string val)
        {
            m_JsonManager.ChangeJsonData(node, val);
        }

        private void SaveJSONFile(object sender, EventArgs e)
        {
            // 여기서 저장하기 전에 TreeView를 JsonData로 갱신할 것!

            m_JsonManager.SaveJsonFile(JSONNameToSave);
        }
    }
}
