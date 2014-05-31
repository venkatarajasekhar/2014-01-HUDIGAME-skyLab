﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using D3D = Microsoft.DirectX.Direct3D;
using Microsoft.DirectX.Direct3D;
using Microsoft.DirectX;

namespace GameTool.Class
{
    class GameObject
    {
        Device m_device;

        private Mesh GameObjectMesh = null;
        D3D.Material[] GameObjectMaterials;
        D3D.Texture[] GameObjectTextures;
        string m_filename;

        public GameObject(string fileName)
        {
            m_filename = fileName;

            if (m_filename.Length == 0)
            {
                System.Windows.Forms.MessageBox.Show("Empty Mesh File Name!");
                return;
            }
        }

        public void init(ref Device d3dDevice)
        {
            m_device = d3dDevice;

            LoadMesh(Class.Renderer.FOLDER_PATH + m_filename, ref GameObjectMesh, ref GameObjectMaterials, ref GameObjectTextures);
        }

        // 메쉬를 불러오는 함수
        private void LoadMesh(string filenameWithPath, ref Mesh mesh, ref Material[] meshmaterials, ref Texture[] meshtextures)
        {
            ExtendedMaterial[] materialarray;
            mesh = Mesh.FromFile(filenameWithPath, MeshFlags.Managed, m_device, out materialarray);

            if ((materialarray != null) && (materialarray.Length > 0))
            {
                meshmaterials = new Material[materialarray.Length];
                meshtextures = new Texture[materialarray.Length];

                for (int i = 0; i < materialarray.Length; i++)
                {
                    meshmaterials[i] = materialarray[i].Material3D;
                    meshmaterials[i].Ambient = meshmaterials[i].Diffuse;

                    if ((materialarray[i].TextureFilename != null) && (materialarray[i].TextureFilename != string.Empty))
                    {
                        meshtextures[i] = TextureLoader.FromFile(m_device, materialarray[i].TextureFilename);
                    }
                }
            }

            mesh = mesh.Clone(mesh.Options.Value, CustomVertex.PositionNormalTextured.Format, m_device);
            mesh.ComputeNormals();

            VertexBuffer vertices = mesh.VertexBuffer;
            GraphicsStream stream = vertices.Lock(0, 0, LockFlags.None);
            vertices.Unlock();
        }

        public void DrawObject()
        {
            DrawMesh(GameObjectMesh, GameObjectMaterials, GameObjectTextures);
        }

        private void ClearMesh()
        {
            GameObjectMaterials = null;
            GameObjectMesh = null;
            GameObjectTextures = null;
        }

        private void DrawMesh(Mesh mesh, Material[] meshmaterials, Texture[] meshtextures)
        {
            for (int i = 0; i < meshmaterials.Length; i++)
            {
                m_device.Material = meshmaterials[i];
                m_device.SetTexture(0, meshtextures[i]);
                mesh.DrawSubset(i);
            }
        }

    }
}
