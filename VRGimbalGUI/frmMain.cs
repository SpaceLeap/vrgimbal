using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VRGimbalGUI
{
    public partial class frmMain : Form
    {
        Dictionary<string, Control> m_params = new Dictionary<string, Control>();
        Dictionary<string, bool> m_paramsChanged = new Dictionary<string, bool>();
        string m_buffer = "";

        float m_degConverter = 1000.0f;

        public frmMain()
        {
            InitializeComponent();

            m_params.Clear();
            //PopulateControlsDictionary(this.Controls);
            
            foreach (Control c in this.Controls)
            {
                PopulateControlsDictionary(c);
                /*
                if (!string.IsNullOrEmpty((string)c.Tag))
                {
                    m_params.Add((string)c.Tag, c);
                    m_paramsChanged.Add((string)c.Tag, false);
                    c.TextChanged += new EventHandler(txtParam_TextChanged);
                }*/
            }
        }

        private void AddControlToDictionary(Control c)
        {
            if (!string.IsNullOrEmpty((string)c.Tag))
            {
                m_params.Add((string)c.Tag, c);
                m_paramsChanged.Add((string)c.Tag, false);
                c.TextChanged += new EventHandler(txtParam_TextChanged);
            }
        }

        private void PopulateControlsDictionary(Control c)
        {
            //if (!string.IsNullOrEmpty((string)c.Tag))
            //{
            //    m_params.Add((string)c.Tag, c);
            //    m_paramsChanged.Add((string)c.Tag, false);
            //    c.TextChanged += new EventHandler(txtParam_TextChanged);
            //}
            AddControlToDictionary(c);
            foreach (Control c2 in c.Controls)
            {
                PopulateControlsDictionary(c2);
            }
        }

        private void gimbalCom_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            if (this.InvokeRequired)
            {
                D_DataReceived d = new D_DataReceived(_DataReceived);
                this.Invoke(d);
            }
            else 
            {
                _DataReceived();
            }
            
        }

        private Single ConvSingle(object str)
        {
            try {
                return Convert.ToSingle(str);
            }
            catch (Exception ex) { }
            return 0;
        }

        private Int32 ConvInt32(object str)
        {
            try
            {
                return Convert.ToInt32(str);
            }
            catch (Exception ex) { }
            return 0;
        }

        delegate void D_DataReceived();
        void _DataReceived()
        {
            string newdata = gimbalCom.ReadExisting();

            m_buffer += newdata;
            char[] rowseps = { '\n', '\r' };
            char [] seps = {' '};
            //decodifico
            int i = m_buffer.IndexOfAny(rowseps);
            int p = -1;
            while (i >= 0)
            {
                
                string row = m_buffer.Substring(p + 1, i - 1 - p);
                string[] fields = row.Split(seps, StringSplitOptions.RemoveEmptyEntries);
                if (fields.Length > 2)
                {
                    if (fields[1] == "ACC")
                    {
                        float roll = ConvSingle(fields[2]) / m_degConverter;
                        float pitch = ConvSingle(fields[0]) / m_degConverter;
                        float yaw = 0;
                        if (fields.Length > 3)
                            yaw = ConvSingle(fields[3]) / m_degConverter;
                        UpdateAttitude(roll, pitch, yaw);
                    }
                }else if (fields.Length > 1)
                {
                    if (m_params.ContainsKey(fields[0]))
                    {
                        Control c = m_params[fields[0]];
                        c.Text = fields[1];
                        TextBox tt = c as TextBox;
                        if ((tt != null) && (!tt.ReadOnly))
                        { 
                            m_paramsChanged[fields[0]] = false;
                            c.BackColor = Color.White;
                        }

                    }
                    if (fields[0] == "vers")
                    {
                        int ver = ConvInt32(fields[1]);
                        if (ver <= 49)
                            m_degConverter = 100.0f;
                        else
                            m_degConverter = 1000.0f;

                        sblblVersion.Text = fields[1];
                    }
                }
                p = i;
                i = m_buffer.IndexOfAny(rowseps, p + 1);
                if (i == p + 1)
                {
                    p++;
                    i = m_buffer.IndexOfAny(rowseps, p + 1);
                }
            }
            if (p > 0)
            {
                m_buffer = m_buffer.Substring(p + 1);
            }

            if (txtLog.Text.Length > 2000)
                txtLog.Text = txtLog.Text.Substring(txtLog.Text.Length - 1000);

            txtLog.Text += newdata;
            txtLog.SelectionStart = txtLog.Text.Length;
            txtLog.ScrollToCaret();
        }

        int cntPts = 0;
        int last_update = 0;
        private void UpdateAttitude(float roll, float pitch, float yaw)
        {
            txtRoll.Text = string.Format("{0:0.00}", roll);
            txtPitch.Text = string.Format("{0:0.00}", pitch);
            txtYaw.Text = string.Format("{0:0.00}", yaw);
            if (Environment.TickCount - last_update > 100)
            {
                last_update = Environment.TickCount;

                attitude.SetAttitudeIndicatorParameters(pitch, roll);
                heading.SetHeadingIndicatorParameters((int) yaw);

                linePitch.AddPoint(cntPts, pitch);
                lineRoll.AddPoint(cntPts, roll);
                if (cntPts > 100)
                {
                    linePitch.RemovePoint(0);
                    lineRoll.RemovePoint(0);
                }
                graphAttitude.GraphPane.XAxis.Scale.Min = cntPts - 100;
                graphAttitude.GraphPane.XAxis.Scale.Max = cntPts;
                graphAttitude.GraphPane.YAxis.Scale.Min = -90;
                graphAttitude.GraphPane.YAxis.Scale.Max = 90;
                graphAttitude.GraphPane.AxisChange();

                graphAttitude.Refresh();

                cntPts++;
            }

        }

        private void timerCom_Tick(object sender, EventArgs e)
        {
            if (gimbalCom.IsOpen)
            {
                cmdOpen.Enabled = false;
                cboComPort.Enabled = false;
                txtBaudRate.Enabled = false;
                cmdClose.Enabled = true;
            }
            else {
                cmdOpen.Enabled = true;
                cboComPort.Enabled = true;
                txtBaudRate.Enabled = true;
                cmdClose.Enabled = false;
            }
        }

        void Send(string str)
        {
            int delay = 1; // 2;
            try
            {
 
                if (delay < 0)
                    gimbalCom.Write(str + "\n");
                else
                {
                    for (int i = 0; i < str.Length; i++)
                    {
                        System.Threading.Thread.Sleep(delay);
                        gimbalCom.Write(str[i] + "");
                    }
                    System.Threading.Thread.Sleep(delay);
                    gimbalCom.Write("\n");
                }

                txtLog.Text += "=> " + str + "\r\n";
                txtLog.SelectionStart = txtLog.Text.Length;
                txtLog.ScrollToCaret();
            }
            catch (Exception ex)
            { }
        }

        private void cmdSend_Click(object sender, EventArgs e)
        {
            Send(txtSend.Text);
        }

        private void cmdOpen_Click(object sender, EventArgs e)
        {
            try
            {
                gimbalCom.PortName = cboComPort.SelectedItem.ToString();
                gimbalCom.BaudRate = ConvInt32(txtBaudRate.Text);
                Connect();

                if (gimbalCom.IsOpen)
                {
                    SaveSettings();

                    Send("OAC 0");
                    Send("par");
                    if (chkOutputACC.Checked)
                        Send("OAC 1");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void cmdClose_Click(object sender, EventArgs e)
        {
            Disconnect();
        }

        private void cmdTC_Click(object sender, EventArgs e)
        {
            Send("TC");
        }

        private void cmdSR_Click(object sender, EventArgs e)
        {
            Send("SR " + txtRoll_KP.Text + " " + txtRoll_KI.Text + " " + txtRoll_KD.Text);
        }

        private void cmdSE_Click(object sender, EventArgs e)
        {
            Send("SE " + txtPitch_PWM.Text + " " + txtRoll_PWM.Text);
        }

        private void cmdSP_Click(object sender, EventArgs e)
        {
            Send("SP " + txtPitch_KP.Text + " " + txtPitch_KI.Text + " " + txtPitch_KD.Text);
        }

        private void cmdSM_Click(object sender, EventArgs e)
        {
            Send("SM  " + txtPitchDir.Text + " " + txtRollDir.Text + " " + txtPitchMotor.Text + " " + txtRollMotor.Text);
        }

        private void cmdSendConfig_Click(object sender, EventArgs e)
        {
            if (gimbalCom.IsOpen)
            {
                /*
                foreach (Control c in this.Controls)
                {
                    if (!string.IsNullOrEmpty((string)c.Tag))
                    {
                        if (m_paramsChanged[(string)c.Tag])
                        {
                            Send("par " + c.Tag + " " + c.Text);
                            //chiedo anche conferma
                            Send("par " + c.Tag );
                        }
                    }
                }*/
                foreach (string key in m_params.Keys)
                {
                    Control c = m_params[key];
                    if (m_paramsChanged[key])
                    {
                        Send("par " + key + " " + c.Text);
                        //chiedo anche conferma
                        Send("par " + key);
                    }
                }
            }
        }

        private void cmdReadConfig_Click(object sender, EventArgs e)
        {
            Send("par");
        }

        private void txtParam_TextChanged(object sender, EventArgs e)
        {
            Control c = sender as Control;
            TextBox tt = c as TextBox;
            if ((tt != null) && (!tt.ReadOnly))
            {
                if (m_paramsChanged.ContainsKey((string)c.Tag))
                {
                    m_paramsChanged[(string)c.Tag] = true;
                    c.BackColor = Color.LightSalmon;
                }
            }
        }

        private void chkOutputACC_CheckedChanged(object sender, EventArgs e)
        {
            if (chkOutputACC.Checked)
                Send("OAC 1");
            else
                Send("OAC 0");
        }

        private void menuExit_Click(object sender, EventArgs e)
        {
            if (CheckCloseApp())
                this.Close();
        }

        private void Connect()
        {
            if (!gimbalCom.IsOpen)
            {
                try
                {

                    this.gimbalCom.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.gimbalCom_DataReceived);
                    gimbalCom.Open();
                }
                catch (Exception ex)
                {
                    this.gimbalCom.DataReceived -= new System.IO.Ports.SerialDataReceivedEventHandler(this.gimbalCom_DataReceived);
                }
            }
        }

        private void Disconnect()
        {
            if (gimbalCom.IsOpen)
            {
                try
                {

                    this.gimbalCom.DataReceived -= new System.IO.Ports.SerialDataReceivedEventHandler(this.gimbalCom_DataReceived);
                    gimbalCom.Close();
                }
                catch (Exception ex)
                { }
            }
        }

        private bool CheckCloseApp()
        {
            bool bClose = false;
            if (gimbalCom.IsOpen)
            {
                if (MessageBox.Show("Disconnect and close?", this.Text, MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    bClose = true;
                }
            } else {
                bClose = true;    
            }
            if (bClose)
            {
                Disconnect();
            }
            return bClose;
        }

        private void menuAbout_Click(object sender, EventArgs e)
        {
            frmAbout f = new frmAbout();
            f.ShowDialog();
        }

        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = !CheckCloseApp();
        }

        private void menuSaveConfig_Click(object sender, EventArgs e)
        {
            if (diagSave.ShowDialog() == DialogResult.OK)
            {
                System.IO.TextWriter tw = new System.IO.StreamWriter(diagSave.FileName);
                foreach (string k in m_params.Keys)
                {
                    tw.WriteLine(k + " " + m_params[k].Text);
                }
                tw.Close();
            }
        }

        private void menuLoadConfig_Click(object sender, EventArgs e)
        {
            if (diagOpen.ShowDialog() == DialogResult.OK)
            {
                System.IO.TextReader tr = new System.IO.StreamReader(diagOpen.FileName);

                string row = tr.ReadLine();

                while (row != null)
                {
                    char[] seps = { ' ' };
                    string[] fields = row.Split(seps, StringSplitOptions.RemoveEmptyEntries);
                    if (fields.Length > 1)
                    {
                        if (m_params.ContainsKey(fields[0]))
                        {
                            Control c = m_params[fields[0]];
                            c.Text = fields[1];
                        }
                    }
                    row = tr.ReadLine();
                }
                
                tr.Close();
            }
        }

        ZedGraph.LineItem lineRoll;
        ZedGraph.LineItem linePitch;
        private void frmMain_Load(object sender, EventArgs e)
        {
            LoadSettings();

            double[] x = { 0, 1, 2, 3 };
            double[] y = { 0,0,0,0 };

            lineRoll = graphAttitude.GraphPane.AddCurve("Roll", x, y, Color.Red, ZedGraph.SymbolType.None);
            linePitch = graphAttitude.GraphPane.AddCurve("Pitch", x, y, Color.Blue, ZedGraph.SymbolType.None);

            graphAttitude.GraphPane.Title.Text = "Attitude";
            graphAttitude.GraphPane.XAxis.Title.Text = "Samples";
            graphAttitude.GraphPane.XAxis.Scale.Min = 0;
            graphAttitude.GraphPane.XAxis.Scale.Max = 100;
            graphAttitude.GraphPane.YAxis.Title.Text = "Degrees";
            graphAttitude.GraphPane.YAxis.Scale.Min = -90;
            graphAttitude.GraphPane.YAxis.Scale.Max = 90;
            graphAttitude.GraphPane.AxisChange();

            graphAttitude.Refresh();
            
        }

        public double ParamValue(string name)
        {
            double ret = 0;
            if (m_params.ContainsKey(name))
            {
                try
                {
                    ret = Convert.ToDouble(m_params[name].Text);
                }
                catch (Exception ex) { }
            }
            return ret;
        }

        private void cmdDefaults_Click(object sender, EventArgs e)
        {
            Send("SD");
            Send("par");
        }

        private void cmdLoadFlash_Click(object sender, EventArgs e)
        {
            Send("RE");
            Send("par");
        }

        private void cmdSaveFlash_Click(object sender, EventArgs e)
        {
            Send("WE");
        }

        private void cmdGyroCal_Click(object sender, EventArgs e)
        {
            Send("GC");
        }

        private void LoadSettings()
        {
            int i;
            string origport = VRGimbalGUI.Properties.Settings.Default.ComPort; //m_port.ComPort;
            string origbaud = VRGimbalGUI.Properties.Settings.Default.BaudRate.ToString(); //m_port.BaudRate.ToString();

            cboComPort.Items.Clear();
            string[] theSerialPortNames = System.IO.Ports.SerialPort.GetPortNames();
            cboComPort.Items.AddRange(theSerialPortNames);

            for (i = 0; i < cboComPort.Items.Count; i++)
            {
                if (origport == (string)cboComPort.Items[i])
                    cboComPort.SelectedIndex = i;
            }

            txtBaudRate.Text = origbaud; 
        }

        private void SaveSettings()
        {
            VRGimbalGUI.Properties.Settings.Default.ComPort = gimbalCom.PortName;
            VRGimbalGUI.Properties.Settings.Default.BaudRate = gimbalCom.BaudRate;
        }
    }
}