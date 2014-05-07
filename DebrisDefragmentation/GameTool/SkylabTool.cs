﻿using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net.Json;

namespace GameTool
{
    public partial class skyLabTool : Form
    {
        // Json Config 파일을 다루기 위한 변수
        private GameTool.Class.JSONInOut JsonControl = new GameTool.Class.JSONInOut();
        // render를 할지 말지 결정하는 bool값
        bool g_IsRenderable = true;

        // Random
        Random r = new Random();

        // Timer
        GameTool.Class.GameTimer m_Timer = null;

        // Renderer
         // DDWrapper의 Renderer가 멤버변수로 DDRenderer*를 가지고 있음
        private DDWrapper.Renderer m_Renderer = new DDWrapper.Renderer();
        
        // Camera
        private GameTool.Class.GameCamera m_Camera = null;

        // Scene
        private GameTool.Class.GameScene m_Scene = null;

        // Player model
        private GameTool.Class.GamePlayer m_Model = null;

        // StopWatch
        System.Diagnostics.Stopwatch m_StopWatch = new System.Diagnostics.Stopwatch();
        
        // acceleration variables
        bool g_IsAccelationInput = false;

        float previousTime = 0;
        float currentTime = 0;

        // light
        private GameTool.Class.GameLight m_Light = null;

        // mouseMovement values
        float m_PrevXPos = 0.0f;
        float m_CurrentXPos = 0.0f;
        float m_PrevYPos = 0.0f;
        float m_CurrentYPos = 0.0f;

        public skyLabTool()
        {
           InitializeComponent();

            // 방어 코드
           if (null == m_Renderer)
            {
                return;
            }
            
            // Direct3D 창에서 마우스 휠 이벤트 추가
           this.View.MouseWheel += new System.Windows.Forms.MouseEventHandler(CameraZoomInOut);

        }

        private void LoadSetting()
        {
            m_Scene = new GameTool.Class.GameScene();

            AddLight();
            LoadMeshes();

            AddCamera();
        }

        private void StartScene(object sender, EventArgs e)
        {
            // Renderer의 오버라이드된 Init 함수를 사용. 윈도우 크기와 HWND를 직접 넘겨준다
            if (m_Renderer.Init(this.View.Handle.ToInt32(), this.View.Width, this.View.Height))
            {
                LoadSetting();

                DrawScreen();

                // Stopwatch 가동
                m_StopWatch.Start();
            }

            // render가 끝난 다음 타이머를 설정한다
            m_Timer = new GameTool.Class.GameTimer(ref TimePass);
        }
        
        // 화면에 그림 그리는 함수
        private async void DrawScreen()
        {
            while(g_IsRenderable)
            {
                m_Renderer.Clear();

                if (m_Renderer.BeginDraw())
                {
                    // 여기서 뭔가 그리게 됩니다
                    MovePlayer();
                    RenderScene();
                }

                m_Renderer.EndDraw();
                UpdateCameraInformation();
                UpdatePlayerStatus();
                await Task.Delay(16);
            }
        }

        // 여기 들어오면 종료된다
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //모든게 끝날 때 D3D도 릴리즈 시켜줘야 합니다
            g_IsRenderable = false;
            m_Renderer.Release();

            Application.Exit();
        }

        // 화면에 오브젝트들 불러오는 함수
        private void LoadMeshes()
        {
            // test character
            string playerPath = "spaceMan.x";
            m_Model = new GameTool.Class.GamePlayer(playerPath);
            m_Model.SetPosition(0, 0, 0);
            m_Scene.AddChild(ref m_Model);

            // test Debris
            string debrisPath = "debris.x";
            float randX, randY, randZ;
            for (int i = 0; i < JsonControl.GetUintVariable(GameTool.Class.ENUM_JSONVAR.DEBRIS_NUMBER); ++i)
            {
                GameTool.Class.GameModel debris = new GameTool.Class.GameModel(debrisPath);
                randX = r.Next(-200, 200);
                randY = r.Next(-200, 200);
                randZ = r.Next(-200, 200);
                debris.SetScale(0.5f);
                debris.SetPosition(randX, randY, randZ);
                m_Scene.AddChild(ref debris);
            }

            // test SkyBox
            string skyboxPath = "skybox.x";
            GameTool.Class.GameModel skybox = new GameTool.Class.GameModel(skyboxPath);
            skybox.SetPosition(0, 0, 0);
            m_Scene.AddChild(ref skybox);

            // test Earth
            string earthPath = "earth.x";
            GameTool.Class.GameModel earth = new GameTool.Class.GameModel(earthPath);
            earth.SetPosition(0, -800, 0);
            m_Scene.AddChild(ref earth);
        }

        private void RenderScene()
        {
            m_Scene.Render();
        }

        private void AddCamera()
        {
            m_Camera = new GameTool.Class.GameCamera(this.View.Size.Width, this.View.Size.Height);
            // 조심해!!
            // 왜 scene 밑에 카메라를 넣으면 화면이 제대로 안 보이는지?;;
            // 현재는 카메라가 혼자 둥둥 떠있는데 FollowingObject가 제대로 안 되고 있고
            // 우주인 외에 아무것도 안 보임 ㄷㄷ
            m_Scene.AddChild(ref m_Camera);
            m_Camera.SetFollowingObject(ref m_Model);
        }

        private void AddLight()
        {
            m_Light = new GameTool.Class.GameLight();
            m_Scene.AddChild(ref m_Light);
        }

        private void UpdateCameraInformation()
        {
            cameraXpos.Text = m_Camera.GetPositionX().ToString();
            cameraYpos.Text = m_Camera.GetPositionY().ToString();
            cameraZpos.Text = m_Camera.GetPositionZ().ToString();

            CameraViewVecX.Text = m_Camera.GetViewDirectionX().ToString();
            CameraViewVecY.Text = m_Camera.GetViewDirectionY().ToString();
            CameraViewVecZ.Text = m_Camera.GetViewDirectionZ().ToString();
        }

        private void ViewMouseEnvet(object sender, EventArgs e)
        {
            this.View.Focus();
        }

        private void ViewMouseLeave(object sender, EventArgs e)
        {
            this.Focus();
        }

        private void ViewMouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                m_CurrentXPos = e.X;
                m_CurrentYPos = e.Y;

                m_Scene.IncreaseRotationX((m_CurrentYPos - m_PrevYPos)/2);
                m_Scene.IncreaseRotationY((m_CurrentXPos - m_PrevXPos)/2);

                m_PrevXPos = m_CurrentXPos;
                m_PrevYPos = m_CurrentYPos;
            }
            else
            {
                // 안쓰면 초기화
                m_CurrentXPos = 0;
                m_CurrentYPos = 0;
                m_PrevXPos = 0;
                m_PrevYPos = 0;
            }
        }

        private void InputProc(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyCode  == Keys.W)
            {
                g_IsAccelationInput = true;
            }
            if (e.KeyCode == Keys.S)
            {
                StopPlayer();
            }
        } 

        private void CameraZoomInOut(object sender, MouseEventArgs e)
        {
            if (e.Delta > 0)
            {
                m_Camera.MoveCamera(0, 0, 1);
            }
            else
            {
                m_Camera.MoveCamera(0, 0, -1);
            }
        }

        private void ResetCamera(object sender, EventArgs e)
        {
            m_Camera.ResetCamera();
        }

        private void StopPlayer()
        {
            m_Model.StopPlayer();
        }

        private void MovePlayer()
        {
            currentTime = m_StopWatch.ElapsedMilliseconds / 1000.0f;
            float dt = currentTime - previousTime;
            previousTime = currentTime;

            if ( !m_Model.MovePlayer(dt, g_IsAccelationInput) )
            {
                g_IsAccelationInput = false;
            }
        }

        private void UpdatePlayerStatus()
        {
            // update position
            PlayerPosX.Text = m_Model.GetPositionX().ToString();
            PlayerPosY.Text = m_Model.GetPositionY().ToString();
            PlayerPosZ.Text = m_Model.GetPositionZ().ToString();

            // update acceleration
            this.IntegratedAccelVal.Text = m_Model.GetAccelation().ToString();
            this.PlayerAccelX.Text = m_Model.GetAccelX().ToString();
            this.PlayerAccelY.Text = m_Model.GetAccelY().ToString();
            this.PlayerAccelZ.Text = m_Model.GetAccelZ().ToString();

            // update speed
            this.IntegratedVelVal.Text = m_Model.GetSpeed().ToString();
            this.PlayerVelocityX.Text = m_Model.GetSpeedX().ToString();
            this.PlayerVelocityY.Text = m_Model.GetSpeedY().ToString();
            this.PlayerVelocityZ.Text = m_Model.GetSpeedZ().ToString();
        }

        private void ResetPlayerStatus(object sender, EventArgs e)
        {
            m_Model.SetPosition(0, 0, 0);
            StopPlayer();
        }

        private void RenderOnOff(object sender, EventArgs e)
        {
            g_IsRenderable = !g_IsRenderable;
            DrawScreen();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////
        //                                       JSON Cobfig Tab                                   //
        /////////////////////////////////////////////////////////////////////////////////////////////

        private void LoadJsonFile(object sender, EventArgs e)
        {
            JsonControl.LoadJsonFile(JsonFileList, JSONVariables, ConfigRestartBtn);
        }

        private void SearchJsonFiles(object sender, EventArgs e)
        {
            JsonControl.SearchJsonFiles(JsonFileList);
        }

        private void SaveJsonFile(object sender, EventArgs e)
        {
            JsonControl.SaveJsonFile(JSONNameToSave);
        }

        private void GetJsonVariable(object sender, EventArgs e)
        {
            string[] fullName = System.Text.RegularExpressions.Regex.Split(JSONVariables.SelectedItem.ToString(), " : ");

            JSONKeyLabel.Text = fullName[0];
            JSONVarBar.Text = fullName[1];
        }

        private void JSONModifyBtn(object sender, EventArgs e)
        {
            JsonControl.JSONModifyBtn(JSONKeyLabel, JSONVarBar, JSONVariables);
        }
    }
}
