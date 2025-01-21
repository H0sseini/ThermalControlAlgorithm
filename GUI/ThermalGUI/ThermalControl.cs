using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Collections.Specialized;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using System.Reflection;

namespace ThermalGUI
{
    public partial class ThermalControl : Form
    {
        public ThermalControl()
        {
            InitializeComponent();
        }

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetValues(int Comp, int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetObjectName(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int getComponentLength();

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void mainFunc();

        

        public static string GetNameFromCpp(int index)
        {
            IntPtr ptr = GetObjectName(index);
            if (ptr == IntPtr.Zero)
                return "Invalid index";
            return Marshal.PtrToStringAnsi(ptr); // Correct conversion for ANSI strings
        }

        private void ReceiveData_Click(object sender, EventArgs e)
        {
            try
            {
                
                // calling the function to retrieve values of selected component
                
                if (ComponentSelect.SelectedIndex>-1)
                {
                    ReceiveData.Enabled = true;
                    int selected = ComponentSelect.SelectedIndex;
                    GetValues(selected, 0)
                    //CompPIDBox.Text = getComponentName(Int32.Parse(CompTemp.Text)).ToString();
                }
                

                
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}");
            }
        }

     



        private void FetchComp_Click(object sender, EventArgs e)
        {
            selectComponentLabel.Text = "Select Component:";
            FetchComp.Enabled = false;
            int Records = getComponentLength();
            for (int i = 0; i < Records; i++)
            {
                string name = GetNameFromCpp(i);
                ComponentSelect.Items.Add(name);
            }
            Task.Run(() =>
            {
                mainFunc(); // This will run continuously without blocking the GUI
            });

            //ReceiveData.Enabled = true;
            //SendData.Enabled = true;

        }
    }
}
