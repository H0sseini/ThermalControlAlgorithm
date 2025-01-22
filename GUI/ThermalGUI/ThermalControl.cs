using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.IO;


namespace ThermalGUI
{

    public partial class ThermalControl : Form
    {
        private bool _running = true; //to control the timer loop
        
       

        string filePath = "./RIDContents.csv";
        
        Dictionary<int, string> RIDList = new Dictionary<int, string>();

        //time to update screen       
        private int TCS_RUN_INTERVAL = 10000;

        public ThermalControl()
        {
            InitializeComponent();
            ManageRIDFile();
            StartEventReporting();
            RecieveAutomaticData();
            Task.Run(() =>
            {
                mainFunc(); // This will run continuously without blocking the GUI
            });
            
            if (RIDs.Text == "")
            {
                eventReport.Enabled = true;
            }
        }

        
        // importing necessary functions to communicate with the .dll file correctly

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr getObjectName(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int getComponentLength();

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void mainFunc();

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompPID(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompTemp(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendCompStatus(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompMaxOpTemp(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompMinOpTemp(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompMaxNopTemp(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompMinNopTemp(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompStartHeatOp(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompStopHeatOp(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompStartHeatNop(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompStopHeatNop(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendValidSensor(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendCompValidHeater(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendCompOBC(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendCompBypass(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendCompTCSState(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendSens1Defined(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens1PID(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendSens1State(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens1OnlineVal(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens1PrevVal(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens1Fault(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendSens2Defined(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens2PID(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendSens2State(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens2OnlineVal(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens2PrevVal(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens2Fault(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendSens3Defined(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens3PID(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendSens3State(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens3OnlineVal(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens3PrevVal(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendSens3Fault(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendHeater1Defined(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendHeater1PID(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendHeater1State(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendHeater1Status(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendHeater2Defined(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int sendHeater2PID(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendHeater2State(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool sendHeater2Status(int index);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ReceiveDataFromGSorOBC(int Comp, bool CompStat, bool OBC, bool Bypass, 
            int Sen1, int Sen2, int Sen3, int HeatStartOp, int HeatStopOp, int HeatStartNop, int HeatStopNop);

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetRIDSize();

        [DllImport("ThermalControlAlgorithm.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetRIDData();


        // defines the string for specific RIDs based on the file given in the current directory
        public void ManageRIDFile()
        {
            string fileName = filePath.Substring(2);
            try
            {
                string[] lines = File.ReadAllLines(filePath);
                foreach (string line in lines)
                {
                    string[] parts = line.Split(',');
                    if (parts.Length == 2 && int.TryParse(parts[0], out int number))
                    {
                        string value = parts[1].Trim();
                        RIDList[number] = value;
                    }
                }
            }
            catch
            {
                RIDs.Text = "Error! There is no file named " + fileName + " in " + System.IO.Path.GetDirectoryName(Application.ExecutablePath) +
                    " or cannot access and/or read and/or parse the file.";
                eventReport.Enabled = false;
            }
        }





        public static string getNameFromCpp(int index)
        {
            IntPtr ptr = getObjectName(index);
            if (ptr == IntPtr.Zero)
                return "Invalid index";
            return Marshal.PtrToStringAnsi(ptr); // Correct conversion for ANSI strings
        }

        private void ReceiveDataFunction()
        {
            try
            {

                // calling the function to retrieve values of selected component

                if (ComponentSelect.SelectedIndex > -1)
                {
                    groupBox9.Enabled = true;

                    // components

                    int selected = ComponentSelect.SelectedIndex;
                    CompPIDBox.Text = sendCompPID(selected).ToString();
                    CompTemp.Text = sendCompTemp(selected).ToString();
                    if (sendCompStatus(selected))
                    {
                        CompStatus.Text = "On";
                    }
                    else
                    {
                        CompStatus.Text = "Off";
                    }
                    CompMaxOpTemp.Text = sendCompMaxOpTemp(selected).ToString();
                    CompMinOpTemp.Text = sendCompMinOpTemp(selected).ToString();
                    CompMaxNopTemp.Text = sendCompMaxNopTemp(selected).ToString();
                    CompMinNopTemp.Text = sendCompMinNopTemp(selected).ToString();
                    CompStartHeatOp.Text = sendCompStartHeatOp(selected).ToString();
                    CompStopHeatOp.Text = sendCompStopHeatOp(selected).ToString();
                    CompStartHeatNop.Text = sendCompStartHeatNop(selected).ToString();
                    CompStopHeatNop.Text = sendCompStopHeatNop(selected).ToString();
                    NoValidSensors.Text = sendValidSensor(selected).ToString();
                    NoValidHeaters.Text = sendCompValidHeater(selected).ToString();
                    if (sendCompOBC(selected))
                    {
                        CompOBC.Text = "OBC-controlled";
                    }
                    else
                    {
                        CompOBC.Text = "Normal";
                    }

                    if (sendCompBypass(selected))
                    {
                        CompTCSBypass.Text = "Bypassed";
                    }
                    else
                    {
                        CompTCSBypass.Text = "Normal";
                    }
                    if (sendCompTCSState(selected))
                    {
                        CompTCSState.Text = "Available";
                    }
                    else
                    {
                        CompTCSState.Text = "No Data";
                    }


                    // Sensors
                    if (sendSens1Defined(selected))
                    {
                        Sensor1Defined.Text = "Defined";
                        Sensor1PID.Text = sendSens1PID(selected).ToString();
                        if (sendSens1State(selected))
                        {
                            Sensor1State.Text = "Ok";
                        }
                        else
                        {
                            Sensor1State.Text = "Faulty";
                        }
                        Sensor1Online.Enabled = true;
                        Sensor1Online.Text = sendSens1OnlineVal(selected).ToString();
                        Sensor1Previous.Text = sendSens1PrevVal(selected).ToString();
                        Sensor1Fault.Text = sendSens1Fault(selected).ToString();
                    }
                    else
                    {
                        Sensor1Defined.Text = "Not Defined";
                        Sensor1PID.Text = "";
                        Sensor1State.Text = "";
                        Sensor1Online.Text = "";
                        Sensor1Previous.Text = "";
                        Sensor1Fault.Text = "";
                        Sensor1Online.Enabled = false;
                    }

                    if (sendSens2Defined(selected))
                    {
                        Sensor2Defined.Text = "Defined";
                        Sensor2PID.Text = sendSens2PID(selected).ToString();
                        if (sendSens2State(selected))
                        {
                            Sensor2State.Text = "Ok";
                        }
                        else
                        {
                            Sensor2State.Text = "Faulty";
                        }
                        Sensor2Online.Enabled = true;
                        Sensor2Online.Text = sendSens2OnlineVal(selected).ToString();
                        Sensor2Previous.Text = sendSens2PrevVal(selected).ToString();
                        Sensor2Fault.Text = sendSens2Fault(selected).ToString();
                    }
                    else
                    {
                        Sensor2Defined.Text = "Not Defined";
                        Sensor2PID.Text = "";
                        Sensor2State.Text = "";
                        Sensor2Online.Text = "";
                        Sensor2Previous.Text = "";
                        Sensor2Fault.Text = "";
                        Sensor2Online.Enabled = false;
                    }

                    if (sendSens3Defined(selected))
                    {
                        Sensor3Defined.Text = "Defined";
                        Sensor3PID.Text = sendSens3PID(selected).ToString();
                        if (sendSens3State(selected))
                        {
                            Sensor3State.Text = "Ok";
                        }
                        else
                        {
                            Sensor3State.Text = "Faulty";
                        }
                        Sensor3Online.Enabled = true;
                        Sensor3Online.Text = sendSens3OnlineVal(selected).ToString();
                        Sensor3Previous.Text = sendSens3PrevVal(selected).ToString();
                        Sensor3Fault.Text = sendSens3Fault(selected).ToString();
                    }
                    else
                    {
                        Sensor3Defined.Text = "Not Defined";
                        Sensor3PID.Text = "";
                        Sensor3State.Text = "";
                        Sensor3Online.Text = "";
                        Sensor3Previous.Text = "";
                        Sensor3Fault.Text = "";
                        Sensor3Online.Enabled = false;
                    }

                    // Heaters
                    if (sendHeater1Defined(selected))
                    {
                        Heater1Defined.Text = "Defined";
                        if (sendHeater1State(selected))
                        {
                            Heater1State.Text = "Healthy";
                        }
                        else
                        {
                            Heater1State.Text = "Failed";
                        }
                        Heater1Status.Enabled = true;
                        if (sendHeater1Status(selected))
                        {
                            Heater1Status.Text = "On";
                        }
                        else
                        {
                            Heater1Status.Text = "Off";
                        }
                        Heater1PID.Text = sendHeater1PID(selected).ToString();
                    }
                    else
                    {
                        Heater1Defined.Text = "Not Defined";
                        Heater1State.Text = "";
                        Heater1Status.Text = "";
                        Heater1PID.Text = "";
                        Heater1Status.Enabled = false;
                    }

                    if (sendHeater2Defined(selected))
                    {
                        Heater2Defined.Text = "Defined";
                        if (sendHeater2State(selected))
                        {
                            Heater2State.Text = "Healthy";
                        }
                        else
                        {
                            Heater2State.Text = "Failed";
                        }
                        Heater2Status.Enabled = true;
                        if (sendHeater2Status(selected))
                        {
                            Heater2Status.Text = "On";
                        }
                        else
                        {
                            Heater2Status.Text = "Off";
                        }
                        Heater2PID.Text = sendHeater2PID(selected).ToString();
                    }
                    else
                    {
                        Heater2Defined.Text = "Not Defined";
                        Heater2State.Text = "";
                        Heater2Status.Text = "";
                        Heater2PID.Text = "";
                        Heater2Status.Enabled = false;
                    }

                    if (Sensor1Online.Enabled)
                    {
                        SendSen1.Enabled = true;
                        SendSen1.Text = "";
                    }
                    else
                    {
                        SendSen1.Enabled = false;
                        SendSen1.Text = "250";
                    }
                    if (Sensor2Online.Enabled)
                    {
                        SendSen2.Enabled = true;
                        SendSen2.Text = "";
                    }
                    else
                    {
                        SendSen2.Enabled = false;
                        SendSen2.Text = "250";
                    }
                    if (Sensor3Online.Enabled)
                    {
                        SendSen3.Enabled = true;
                        SendSen3.Text = "";
                    }
                    else
                    {
                        SendSen3.Enabled = false;
                        SendSen3.Text = "250";
                    }

                    if (Heater1Status.Enabled)
                    {
                        SendStartHOp.Enabled = true;
                        SendStopHOp.Enabled = true;
                        SendStartHNop.Enabled = true;
                        SendStopHNop.Enabled = true;


                    }
                    else
                    {
                        SendStartHOp.Enabled = false;
                        SendStopHOp.Enabled = false;
                        SendStartHNop.Enabled = false;
                        SendStopHNop.Enabled = false;

                    }

                }

            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}");
            }
        }

        private void ReceiveData_Click(object sender, EventArgs e)
        {
            ReceiveDataFunction();
        }

     



        private void FetchComp_Click(object sender, EventArgs e)
        {
            selectComponentLabel.Text = "Select Component:";
            FetchComp.Enabled = false;
            int Records = getComponentLength();
            for (int i = 0; i < Records; i++)
            {
                string name = getNameFromCpp(i);

                ComponentSelect.Items.Add(name);
            }

        }

        private void ComponentSelect_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ComponentSelect.SelectedIndex > -1)
            {
                ReceiveData.Enabled = true;
                
            }
        }



        private void SendData_Click(object sender, EventArgs e)
        {
            //int Comp, bool CompStat, bool OBC, bool Bypass, int Sen1, int Sen2, int Sen3, int start heat op, int stop heater op and nop;
            
            if (ComponentSelect.SelectedIndex > -1)
            {
                int Comp = ComponentSelect.SelectedIndex;
                

                bool CompStat = false;
                bool OBCCommand = false;
                bool OBC = false;
                int Sen1 = 250;
                int Sen2 = 250;
                int Sen3 = 250;
                int HeatStartOp = 0;
                int HeatStopOp = 50;
                int HeatStartNop = -100;
                int HeatStopNop = -50;
                if (SendCompStat.SelectedIndex == 0)
                {
                    CompStat = true;
                }
                if (SendOBC.SelectedIndex == 0)
                {
                    OBCCommand = true; // this is for when obc asks to turn a component but the component needs to warm up
                }
                if (SendBypass.SelectedIndex == 0)
                {
                    OBC = true; // this is when obc or gs asks to turn off FDIR (tcs algorithm) for a specific module
                }
                //if (SendHeat1.SelectedIndex == 0)
                //{
                //    heat1 = true;
                //}
                //if (SendHeat2.SelectedIndex == 0)
                //{
                //    heat2 = true;
                //}
                

                try
                {
                    Sen1 = Int32.Parse(SendSen1.Text);
                }
                catch 
                {
                    Sen1 = Int32.Parse(Sensor1Online.Text);
                }
                try
                {
                    Sen2 = Int32.Parse(SendSen2.Text);
                }
                catch 
                {
                    Sen2 = Int32.Parse(Sensor2Online.Text);
                }
                try
                {
                    Sen3 = Int32.Parse(SendSen3.Text);
                }
                catch
                {
                    Sen3 = Int32.Parse(Sensor3Online.Text);
                }

                try
                {
                    HeatStartOp = Int32.Parse(SendStartHOp.Text);
                }
                catch
                {
                    HeatStartOp = Int32.Parse(CompStartHeatOp.Text);
                }

                try
                {
                    HeatStopOp = Int32.Parse(SendStopHOp.Text);
                }
                catch
                {
                    HeatStopOp = Int32.Parse(CompStopHeatOp.Text);
                }

                try
                {
                    HeatStartNop = Int32.Parse(SendStartHNop.Text);
                }
                catch
                {
                    HeatStartNop = Int32.Parse(CompStartHeatNop.Text);
                }

                try
                {
                    HeatStopNop = Int32.Parse(SendStopHNop.Text);
                }
                catch
                {
                    HeatStopNop = Int32.Parse(CompStopHeatNop.Text);
                }


                ReceiveDataFromGSorOBC(Comp, CompStat, OBCCommand, OBC, Sen1, Sen2, Sen3, HeatStartOp, HeatStopOp, HeatStartNop, HeatStopNop);
            }
        }

        private void GettingEvents()
        {
            RIDs.Text = "";
            int Size = GetRIDSize();
            IntPtr RIDRawReport = GetRIDData();

            int[] RIDReport = new int[Size];
            Marshal.Copy(RIDRawReport, RIDReport, 0, Size);

            for (int i = 0; i < Size; i++)
            {
                if (!(RIDReport[i] == -1))
                {
                    RIDs.Text += RIDList[RIDReport[i]];
                    RIDs.Text += ";\t";
                }
            }
        }
        private void EventReport_Click(object sender, EventArgs e)
        {
            GettingEvents();
        }
        private async void StartEventReporting()
        {
            while (_running)
            {
                await Task.Delay(15000); //every 15 seconds report the event
                GettingEvents();
            }
        }

        private async void RecieveAutomaticData()
        {
            while (_running)
            {
                await Task.Delay(TCS_RUN_INTERVAL);    //every 10 seconds fetch data
                ReceiveDataFunction();
            }
        }

        private void ThermalControl_FormClosing(object sender, FormClosedEventArgs e)
        {
            _running = false;
        }
    }


}
