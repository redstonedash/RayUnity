using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Runtime.InteropServices;
using System;
using Unity.Collections.LowLevel.Unsafe;

[InitializeOnLoad]
public class RaySceneEditor
{
    
    [DllImport("IPCdatachunk")]
    private static extern IntPtr GetData();
    [DllImport("IPCdatachunk")]
    private static extern void ZeroData(); //dosn't seem to work lmao
    static IntPtr data;
    static IntPtr matrixData;
    static Matrix4x4[] matrix4X4s;
    static Transform[] everything;
    static byte[] datablock = new byte[Marshal.SizeOf(typeof(Matrix4x4))];
    static RaySceneEditor()
    {
        Debug.Log("Up and running");
        data = GetData();
        matrixData = data + Marshal.SizeOf<int>();
        EditorApplication.playModeStateChanged += StateChanged;
        EditorApplication.update += Update;
    }
    static void Update()
    {
        //bizzbuzz = GameObject.Find("ray").transform;
        //byte[] cstring = new byte[8];
        //float[] foobar = { bizzbuzz.position.x*100, bizzbuzz.position.y*100 };
        //Buffer.BlockCopy(foobar, 0, cstring, 0, 8);
        //Marshal.Copy(cstring, 0, data, cstring.Length);
        //Debug.Log(Marshal.PtrToStructure(data,typeof(Vector2)));
        if (Application.isPlaying) {
            //Buffer.BlockCopy(matrix4X4s,0,datablock, 0, Marshal.SizeOf(typeof(Matrix4x4))*matrix4X4s.Length);
            //Marshal.Copy(datablock, 0, data, datablock.Length);
            unsafe
            {
                for (int i = 0; i < matrix4X4s.Length; i++)
                {
                    //matrix4X4s[i] = everything[i].worldToLocalMatrix;
                    if (everything[i].hasChanged)
                    {
                        matrix4X4s[i] = swapBetweenRaylibAndUnitySpace(everything[i].localToWorldMatrix,false);
                        //insert matrix into natice array
                        SetNativeMatrix(matrix4X4s[i], matrixData, i);
                        Debug.Log("updated");
                        
                    }
                    else
                    {
                        
                        Matrix4x4 m = new Matrix4x4();
                        
                        GetNativeMatrix(out m, matrixData, i);
                        m = swapBetweenRaylibAndUnitySpace(m,true);

                        everything[i].position = m.GetColumn(3);

                        // Extract new local rotation
                        everything[i].rotation = Quaternion.LookRotation(
                            m.GetColumn(2),
                            m.GetColumn(1)
                        );

                        // Extract new local scale
                        everything[i].localScale = new Vector3(
                            m.GetColumn(0).magnitude,
                            m.GetColumn(1).magnitude,
                            m.GetColumn(2).magnitude
                        );
                        matrix4X4s[i] = m;

                        //get the native matrix and make it not native and apply it to a transform
                    }
                    everything[i].hasChanged = false;
                }
                //SetNativeVertexArray(matrix4X4s, data);
            }
        }
    }
    static void StateChanged(PlayModeStateChange state)
    {
        if (state == PlayModeStateChange.EnteredPlayMode)
        {
            //send over all initial data for now just transforms
            everything = GameObject.FindObjectsOfType<Transform>();
            Marshal.StructureToPtr<int>(everything.Length, data, false);
            matrix4X4s = new Matrix4x4[everything.Length];
            for(int i = 0; i < matrix4X4s.Length; i++)
            {
                matrix4X4s[i] = swapBetweenRaylibAndUnitySpace(everything[i].localToWorldMatrix, false);
            }
            SetNativeMatrixArray(matrix4X4s, matrixData);
        } else if (state == PlayModeStateChange.ExitingPlayMode)
        {
            ZeroData(); //clears the game state, this will also cause raylib to terminate, hopefully gracefully
            
        }
    }
    static unsafe void SetNativeMatrixArray(Matrix4x4[] vertexArray, IntPtr vertexBuffer)
    {
        // pin the target vertex array and get a pointer to it
        fixed (void* vertexArrayPointer = vertexArray)
        {
            // memcopy the native array over the top
            UnsafeUtility.MemCpy(vertexBuffer.ToPointer(), vertexArrayPointer, vertexArray.Length * (long)UnsafeUtility.SizeOf<Matrix4x4>());
        }
    }
    static unsafe void SetNativeMatrix(Matrix4x4 matrix, IntPtr matrixArray, int arrayIndex)
    {
        Marshal.StructureToPtr(matrix, matrixArray + arrayIndex * Marshal.SizeOf(typeof(Matrix4x4)), false);
    }
    static unsafe void GetNativeMatrix(out Matrix4x4 matrix, IntPtr matrixArray, int arrayIndex)
    {
        matrix = Marshal.PtrToStructure<Matrix4x4>(matrixArray + arrayIndex * Marshal.SizeOf(typeof(Matrix4x4)));
    }
    static Matrix4x4 swapBetweenRaylibAndUnitySpace(Matrix4x4 transformMatrix, bool reciving)
    {
        Matrix4x4 m = (Matrix4x4.Scale(new Vector3(-1, -1, -1)) * transformMatrix.transpose * new Matrix4x4(
                        new Vector4(1, 0, 0, 0),
                        new Vector4(0, 1, 0, 0),
                        new Vector4(0, 0, -1, 0),
                        new Vector4(0, 0, 0, 1)))
                        ;
        if (reciving) {
            m.m02 *= -1;
            m.m03 *= -1;
            m.m12 *= -1;
            m.m13 *= -1;
            m.m20 *= -1;
            m.m21 *= -1;
            m.m30 *= -1;
            m.m31 *= -1;
        }
        return m;
    }
}

