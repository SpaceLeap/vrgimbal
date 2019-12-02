namespace VRGimbalGUI
{
    partial class frmMain
    {
        /// <summary>
        /// Variabile di progettazione necessaria.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Liberare le risorse in uso.
        /// </summary>
        /// <param name="disposing">ha valore true se le risorse gestite devono essere eliminate, false in caso contrario.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Codice generato da Progettazione Windows Form

        /// <summary>
        /// Metodo necessario per il supporto della finestra di progettazione. Non modificare
        /// il contenuto del metodo con l'editor di codice.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.gimbalCom = new System.IO.Ports.SerialPort(this.components);
            this.timerCom = new System.Windows.Forms.Timer(this.components);
            this.cmdOpen = new System.Windows.Forms.Button();
            this.cmdClose = new System.Windows.Forms.Button();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.cmdSend = new System.Windows.Forms.Button();
            this.txtSend = new System.Windows.Forms.TextBox();
            this.cmdTC = new System.Windows.Forms.Button();
            this.cmdSR = new System.Windows.Forms.Button();
            this.txtRoll_KD = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtRoll_KI = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtRoll_KP = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtRoll_PWM = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.txtPitch_PWM = new System.Windows.Forms.TextBox();
            this.cmdSE = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.txtPitch_KP = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.txtPitch_KI = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.txtPitch_KD = new System.Windows.Forms.TextBox();
            this.cmdSP = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.txtRollMotor = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.txtRollDir = new System.Windows.Forms.TextBox();
            this.cmdSM = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.txtPitchMotor = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.txtPitchDir = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label14 = new System.Windows.Forms.Label();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.cmdSendConfig = new System.Windows.Forms.Button();
            this.cmdReadConfig = new System.Windows.Forms.Button();
            this.grpRoll = new System.Windows.Forms.GroupBox();
            this.grpPitch = new System.Windows.Forms.GroupBox();
            this.chkOutputACC = new System.Windows.Forms.CheckBox();
            this.grpAttitude = new System.Windows.Forms.GroupBox();
            this.txtYaw = new System.Windows.Forms.TextBox();
            this.label19 = new System.Windows.Forms.Label();
            this.txtPitch = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.txtRoll = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.grpIMU = new System.Windows.Forms.GroupBox();
            this.textBox6 = new System.Windows.Forms.TextBox();
            this.label21 = new System.Windows.Forms.Label();
            this.textBox7 = new System.Windows.Forms.TextBox();
            this.label22 = new System.Windows.Forms.Label();
            this.attitude = new AvionicsInstrumentControlDemo.AttitudeIndicatorInstrumentControl();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.statusBar = new System.Windows.Forms.StatusStrip();
            this.menuMain = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuLoadConfig = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSaveConfig = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.menuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.menuHelp = new System.Windows.Forms.ToolStripMenuItem();
            this.menuAbout = new System.Windows.Forms.ToolStripMenuItem();
            this.diagSave = new System.Windows.Forms.SaveFileDialog();
            this.diagOpen = new System.Windows.Forms.OpenFileDialog();
            this.graphAttitude = new ZedGraph.ZedGraphControl();
            this.heading = new AvionicsInstrumentControlDemo.HeadingIndicatorInstrumentControl();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.sblblVersion = new System.Windows.Forms.ToolStripStatusLabel();
            this.cmdDefaults = new System.Windows.Forms.Button();
            this.cmdLoadFlash = new System.Windows.Forms.Button();
            this.cmdGyroCal = new System.Windows.Forms.Button();
            this.cmdSaveFlash = new System.Windows.Forms.Button();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.cboComPort = new System.Windows.Forms.ComboBox();
            this.txtBaudRate = new System.Windows.Forms.TextBox();
            this.grpRoll.SuspendLayout();
            this.grpPitch.SuspendLayout();
            this.grpAttitude.SuspendLayout();
            this.grpIMU.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.statusBar.SuspendLayout();
            this.menuMain.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // gimbalCom
            // 
            this.gimbalCom.BaudRate = 115200;
            // 
            // timerCom
            // 
            this.timerCom.Enabled = true;
            this.timerCom.Tick += new System.EventHandler(this.timerCom_Tick);
            // 
            // cmdOpen
            // 
            this.cmdOpen.Location = new System.Drawing.Point(313, 82);
            this.cmdOpen.Name = "cmdOpen";
            this.cmdOpen.Size = new System.Drawing.Size(76, 23);
            this.cmdOpen.TabIndex = 0;
            this.cmdOpen.Text = "Open";
            this.cmdOpen.UseVisualStyleBackColor = true;
            this.cmdOpen.Click += new System.EventHandler(this.cmdOpen_Click);
            // 
            // cmdClose
            // 
            this.cmdClose.Location = new System.Drawing.Point(313, 111);
            this.cmdClose.Name = "cmdClose";
            this.cmdClose.Size = new System.Drawing.Size(76, 23);
            this.cmdClose.TabIndex = 1;
            this.cmdClose.Text = "Close";
            this.cmdClose.UseVisualStyleBackColor = true;
            this.cmdClose.Click += new System.EventHandler(this.cmdClose_Click);
            // 
            // txtLog
            // 
            this.txtLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtLog.Location = new System.Drawing.Point(0, 460);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtLog.Size = new System.Drawing.Size(792, 81);
            this.txtLog.TabIndex = 16;
            // 
            // cmdSend
            // 
            this.cmdSend.Location = new System.Drawing.Point(739, 200);
            this.cmdSend.Name = "cmdSend";
            this.cmdSend.Size = new System.Drawing.Size(101, 23);
            this.cmdSend.TabIndex = 15;
            this.cmdSend.Text = "Send command";
            this.cmdSend.UseVisualStyleBackColor = true;
            this.cmdSend.Visible = false;
            this.cmdSend.Click += new System.EventHandler(this.cmdSend_Click);
            // 
            // txtSend
            // 
            this.txtSend.Location = new System.Drawing.Point(640, 203);
            this.txtSend.Name = "txtSend";
            this.txtSend.Size = new System.Drawing.Size(180, 20);
            this.txtSend.TabIndex = 14;
            this.txtSend.Visible = false;
            // 
            // cmdTC
            // 
            this.cmdTC.Location = new System.Drawing.Point(767, 224);
            this.cmdTC.Name = "cmdTC";
            this.cmdTC.Size = new System.Drawing.Size(73, 22);
            this.cmdTC.TabIndex = 2;
            this.cmdTC.Text = "TC";
            this.cmdTC.UseVisualStyleBackColor = true;
            this.cmdTC.Visible = false;
            this.cmdTC.Click += new System.EventHandler(this.cmdTC_Click);
            // 
            // cmdSR
            // 
            this.cmdSR.Location = new System.Drawing.Point(767, 252);
            this.cmdSR.Name = "cmdSR";
            this.cmdSR.Size = new System.Drawing.Size(73, 22);
            this.cmdSR.TabIndex = 6;
            this.cmdSR.Text = "SR";
            this.cmdSR.UseVisualStyleBackColor = true;
            this.cmdSR.Visible = false;
            this.cmdSR.Click += new System.EventHandler(this.cmdSR_Click);
            // 
            // txtRoll_KD
            // 
            this.txtRoll_KD.Location = new System.Drawing.Point(215, 19);
            this.txtRoll_KD.Name = "txtRoll_KD";
            this.txtRoll_KD.Size = new System.Drawing.Size(62, 20);
            this.txtRoll_KD.TabIndex = 5;
            this.txtRoll_KD.Tag = "gyroRollKd";
            this.txtRoll_KD.Text = "30000";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(195, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(15, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "D";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(100, 23);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(10, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "I";
            // 
            // txtRoll_KI
            // 
            this.txtRoll_KI.Location = new System.Drawing.Point(121, 19);
            this.txtRoll_KI.Name = "txtRoll_KI";
            this.txtRoll_KI.Size = new System.Drawing.Size(62, 20);
            this.txtRoll_KI.TabIndex = 4;
            this.txtRoll_KI.Tag = "gyroRollKi";
            this.txtRoll_KI.Text = "25000";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 23);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(14, 13);
            this.label3.TabIndex = 12;
            this.label3.Text = "P";
            // 
            // txtRoll_KP
            // 
            this.txtRoll_KP.Location = new System.Drawing.Point(32, 19);
            this.txtRoll_KP.Name = "txtRoll_KP";
            this.txtRoll_KP.Size = new System.Drawing.Size(62, 20);
            this.txtRoll_KP.TabIndex = 3;
            this.txtRoll_KP.Tag = "gyroRollKp";
            this.txtRoll_KP.Text = "20000";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(95, 75);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(108, 13);
            this.label4.TabIndex = 17;
            this.label4.Text = "PWM power (0 - 255)";
            // 
            // txtRoll_PWM
            // 
            this.txtRoll_PWM.Location = new System.Drawing.Point(215, 72);
            this.txtRoll_PWM.Name = "txtRoll_PWM";
            this.txtRoll_PWM.Size = new System.Drawing.Size(24, 20);
            this.txtRoll_PWM.TabIndex = 11;
            this.txtRoll_PWM.Tag = "maxPWMmotorRoll";
            this.txtRoll_PWM.Text = "80";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(95, 75);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(108, 13);
            this.label5.TabIndex = 15;
            this.label5.Text = "PWM power (0 - 255)";
            // 
            // txtPitch_PWM
            // 
            this.txtPitch_PWM.Location = new System.Drawing.Point(215, 72);
            this.txtPitch_PWM.Name = "txtPitch_PWM";
            this.txtPitch_PWM.Size = new System.Drawing.Size(24, 20);
            this.txtPitch_PWM.TabIndex = 12;
            this.txtPitch_PWM.Tag = "maxPWMmotorPitch";
            this.txtPitch_PWM.Text = "80";
            // 
            // cmdSE
            // 
            this.cmdSE.Location = new System.Drawing.Point(767, 353);
            this.cmdSE.Name = "cmdSE";
            this.cmdSE.Size = new System.Drawing.Size(73, 22);
            this.cmdSE.TabIndex = 13;
            this.cmdSE.Text = "SE";
            this.cmdSE.UseVisualStyleBackColor = true;
            this.cmdSE.Visible = false;
            this.cmdSE.Click += new System.EventHandler(this.cmdSE_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 23);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(14, 13);
            this.label6.TabIndex = 24;
            this.label6.Text = "P";
            // 
            // txtPitch_KP
            // 
            this.txtPitch_KP.Location = new System.Drawing.Point(32, 19);
            this.txtPitch_KP.Name = "txtPitch_KP";
            this.txtPitch_KP.Size = new System.Drawing.Size(62, 20);
            this.txtPitch_KP.TabIndex = 7;
            this.txtPitch_KP.Tag = "gyroPitchKp";
            this.txtPitch_KP.Text = "20000";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(100, 23);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(10, 13);
            this.label7.TabIndex = 22;
            this.label7.Text = "I";
            // 
            // txtPitch_KI
            // 
            this.txtPitch_KI.Location = new System.Drawing.Point(121, 19);
            this.txtPitch_KI.Name = "txtPitch_KI";
            this.txtPitch_KI.Size = new System.Drawing.Size(62, 20);
            this.txtPitch_KI.TabIndex = 8;
            this.txtPitch_KI.Tag = "gyroPitchKi";
            this.txtPitch_KI.Text = "25000";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(195, 23);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(15, 13);
            this.label8.TabIndex = 20;
            this.label8.Text = "D";
            // 
            // txtPitch_KD
            // 
            this.txtPitch_KD.Location = new System.Drawing.Point(215, 19);
            this.txtPitch_KD.Name = "txtPitch_KD";
            this.txtPitch_KD.Size = new System.Drawing.Size(62, 20);
            this.txtPitch_KD.TabIndex = 9;
            this.txtPitch_KD.Tag = "gyroPitchKd";
            this.txtPitch_KD.Text = "30000";
            // 
            // cmdSP
            // 
            this.cmdSP.Location = new System.Drawing.Point(767, 281);
            this.cmdSP.Name = "cmdSP";
            this.cmdSP.Size = new System.Drawing.Size(73, 22);
            this.cmdSP.TabIndex = 10;
            this.cmdSP.Text = "SP";
            this.cmdSP.UseVisualStyleBackColor = true;
            this.cmdSP.Visible = false;
            this.cmdSP.Click += new System.EventHandler(this.cmdSP_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(15, 49);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(62, 13);
            this.label9.TabIndex = 31;
            this.label9.Text = "Motor Num.";
            // 
            // txtRollMotor
            // 
            this.txtRollMotor.Location = new System.Drawing.Point(76, 45);
            this.txtRollMotor.Name = "txtRollMotor";
            this.txtRollMotor.Size = new System.Drawing.Size(25, 20);
            this.txtRollMotor.TabIndex = 25;
            this.txtRollMotor.Tag = "motorNumberRoll";
            this.txtRollMotor.Text = "0";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(115, 49);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(88, 13);
            this.label10.TabIndex = 30;
            this.label10.Text = "Motor Dir. (-1 / 1)";
            // 
            // txtRollDir
            // 
            this.txtRollDir.Location = new System.Drawing.Point(215, 46);
            this.txtRollDir.Name = "txtRollDir";
            this.txtRollDir.Size = new System.Drawing.Size(24, 20);
            this.txtRollDir.TabIndex = 26;
            this.txtRollDir.Tag = "dirMotorRoll";
            this.txtRollDir.Text = "1";
            // 
            // cmdSM
            // 
            this.cmdSM.Location = new System.Drawing.Point(767, 309);
            this.cmdSM.Name = "cmdSM";
            this.cmdSM.Size = new System.Drawing.Size(73, 22);
            this.cmdSM.TabIndex = 28;
            this.cmdSM.Text = "SM";
            this.cmdSM.UseVisualStyleBackColor = true;
            this.cmdSM.Visible = false;
            this.cmdSM.Click += new System.EventHandler(this.cmdSM_Click);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(7, 49);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(62, 13);
            this.label11.TabIndex = 35;
            this.label11.Text = "Motor Num.";
            // 
            // txtPitchMotor
            // 
            this.txtPitchMotor.Location = new System.Drawing.Point(75, 45);
            this.txtPitchMotor.Name = "txtPitchMotor";
            this.txtPitchMotor.Size = new System.Drawing.Size(25, 20);
            this.txtPitchMotor.TabIndex = 32;
            this.txtPitchMotor.Tag = "motorNumberPitch";
            this.txtPitchMotor.Text = "1";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(115, 48);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(88, 13);
            this.label12.TabIndex = 34;
            this.label12.Text = "Motor Dir. (-1 / 1)";
            // 
            // txtPitchDir
            // 
            this.txtPitchDir.Location = new System.Drawing.Point(215, 46);
            this.txtPitchDir.Name = "txtPitchDir";
            this.txtPitchDir.Size = new System.Drawing.Size(24, 20);
            this.txtPitchDir.TabIndex = 33;
            this.txtPitchDir.Tag = "dirMotorPitch";
            this.txtPitchDir.Text = "1";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(6, 23);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(90, 13);
            this.label13.TabIndex = 39;
            this.label13.Text = "accTimeConstant";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(101, 19);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(62, 20);
            this.textBox1.TabIndex = 36;
            this.textBox1.Tag = "accTimeConstant";
            this.textBox1.Text = "7";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(166, 23);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(46, 13);
            this.label14.TabIndex = 38;
            this.label14.Text = "mpuLPF";
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(233, 19);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(62, 20);
            this.textBox2.TabIndex = 37;
            this.textBox2.Tag = "mpuLPF";
            this.textBox2.Text = "0";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(6, 49);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(61, 13);
            this.label15.TabIndex = 43;
            this.label15.Text = "enableGyro";
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(101, 45);
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(62, 20);
            this.textBox3.TabIndex = 40;
            this.textBox3.Tag = "enableGyro";
            this.textBox3.Text = "1";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(166, 49);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(60, 13);
            this.label16.TabIndex = 42;
            this.label16.Text = "enableACC";
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(233, 45);
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(62, 20);
            this.textBox4.TabIndex = 41;
            this.textBox4.Tag = "enableACC";
            this.textBox4.Text = "1";
            // 
            // cmdSendConfig
            // 
            this.cmdSendConfig.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdSendConfig.Location = new System.Drawing.Point(132, 0);
            this.cmdSendConfig.Margin = new System.Windows.Forms.Padding(0);
            this.cmdSendConfig.Name = "cmdSendConfig";
            this.cmdSendConfig.Size = new System.Drawing.Size(132, 26);
            this.cmdSendConfig.TabIndex = 44;
            this.cmdSendConfig.Text = "Send Config";
            this.cmdSendConfig.UseVisualStyleBackColor = true;
            this.cmdSendConfig.Click += new System.EventHandler(this.cmdSendConfig_Click);
            // 
            // cmdReadConfig
            // 
            this.cmdReadConfig.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdReadConfig.Location = new System.Drawing.Point(0, 0);
            this.cmdReadConfig.Margin = new System.Windows.Forms.Padding(0);
            this.cmdReadConfig.Name = "cmdReadConfig";
            this.cmdReadConfig.Size = new System.Drawing.Size(132, 26);
            this.cmdReadConfig.TabIndex = 45;
            this.cmdReadConfig.Text = "Read Config";
            this.cmdReadConfig.UseVisualStyleBackColor = true;
            this.cmdReadConfig.Click += new System.EventHandler(this.cmdReadConfig_Click);
            // 
            // grpRoll
            // 
            this.grpRoll.Controls.Add(this.txtRoll_KD);
            this.grpRoll.Controls.Add(this.label1);
            this.grpRoll.Controls.Add(this.txtRoll_KI);
            this.grpRoll.Controls.Add(this.label2);
            this.grpRoll.Controls.Add(this.txtRoll_KP);
            this.grpRoll.Controls.Add(this.label3);
            this.grpRoll.Controls.Add(this.txtRollDir);
            this.grpRoll.Controls.Add(this.label10);
            this.grpRoll.Controls.Add(this.txtRollMotor);
            this.grpRoll.Controls.Add(this.label9);
            this.grpRoll.Controls.Add(this.txtRoll_PWM);
            this.grpRoll.Controls.Add(this.label4);
            this.grpRoll.Location = new System.Drawing.Point(14, 25);
            this.grpRoll.Name = "grpRoll";
            this.grpRoll.Size = new System.Drawing.Size(290, 103);
            this.grpRoll.TabIndex = 46;
            this.grpRoll.TabStop = false;
            this.grpRoll.Text = "Roll";
            // 
            // grpPitch
            // 
            this.grpPitch.Controls.Add(this.txtPitch_KD);
            this.grpPitch.Controls.Add(this.label8);
            this.grpPitch.Controls.Add(this.txtPitch_KI);
            this.grpPitch.Controls.Add(this.label7);
            this.grpPitch.Controls.Add(this.txtPitch_KP);
            this.grpPitch.Controls.Add(this.label6);
            this.grpPitch.Controls.Add(this.txtPitchDir);
            this.grpPitch.Controls.Add(this.label12);
            this.grpPitch.Controls.Add(this.txtPitchMotor);
            this.grpPitch.Controls.Add(this.label11);
            this.grpPitch.Controls.Add(this.txtPitch_PWM);
            this.grpPitch.Controls.Add(this.label5);
            this.grpPitch.Location = new System.Drawing.Point(14, 130);
            this.grpPitch.Name = "grpPitch";
            this.grpPitch.Size = new System.Drawing.Size(292, 99);
            this.grpPitch.TabIndex = 47;
            this.grpPitch.TabStop = false;
            this.grpPitch.Text = "Pitch";
            // 
            // chkOutputACC
            // 
            this.chkOutputACC.AutoSize = true;
            this.chkOutputACC.Location = new System.Drawing.Point(10, 19);
            this.chkOutputACC.Name = "chkOutputACC";
            this.chkOutputACC.Size = new System.Drawing.Size(97, 17);
            this.chkOutputACC.TabIndex = 48;
            this.chkOutputACC.Text = "Output Attitude";
            this.chkOutputACC.UseVisualStyleBackColor = true;
            this.chkOutputACC.CheckedChanged += new System.EventHandler(this.chkOutputACC_CheckedChanged);
            // 
            // grpAttitude
            // 
            this.grpAttitude.Controls.Add(this.txtYaw);
            this.grpAttitude.Controls.Add(this.label19);
            this.grpAttitude.Controls.Add(this.txtPitch);
            this.grpAttitude.Controls.Add(this.label17);
            this.grpAttitude.Controls.Add(this.txtRoll);
            this.grpAttitude.Controls.Add(this.label18);
            this.grpAttitude.Controls.Add(this.chkOutputACC);
            this.grpAttitude.Location = new System.Drawing.Point(14, 340);
            this.grpAttitude.Name = "grpAttitude";
            this.grpAttitude.Size = new System.Drawing.Size(296, 74);
            this.grpAttitude.TabIndex = 49;
            this.grpAttitude.TabStop = false;
            this.grpAttitude.Text = "Attitude";
            // 
            // txtYaw
            // 
            this.txtYaw.Location = new System.Drawing.Point(231, 38);
            this.txtYaw.Name = "txtYaw";
            this.txtYaw.ReadOnly = true;
            this.txtYaw.Size = new System.Drawing.Size(46, 20);
            this.txtYaw.TabIndex = 53;
            this.txtYaw.Tag = "";
            this.txtYaw.Text = "0.00";
            this.txtYaw.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(194, 42);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(28, 13);
            this.label19.TabIndex = 54;
            this.label19.Text = "Yaw";
            // 
            // txtPitch
            // 
            this.txtPitch.Location = new System.Drawing.Point(137, 38);
            this.txtPitch.Name = "txtPitch";
            this.txtPitch.ReadOnly = true;
            this.txtPitch.Size = new System.Drawing.Size(46, 20);
            this.txtPitch.TabIndex = 50;
            this.txtPitch.Tag = "";
            this.txtPitch.Text = "0.00";
            this.txtPitch.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(100, 42);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(31, 13);
            this.label17.TabIndex = 51;
            this.label17.Text = "Pitch";
            // 
            // txtRoll
            // 
            this.txtRoll.Location = new System.Drawing.Point(42, 38);
            this.txtRoll.Name = "txtRoll";
            this.txtRoll.ReadOnly = true;
            this.txtRoll.Size = new System.Drawing.Size(52, 20);
            this.txtRoll.TabIndex = 49;
            this.txtRoll.Tag = "";
            this.txtRoll.Text = "0.00";
            this.txtRoll.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(11, 42);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(25, 13);
            this.label18.TabIndex = 52;
            this.label18.Text = "Roll";
            // 
            // grpIMU
            // 
            this.grpIMU.Controls.Add(this.textBox6);
            this.grpIMU.Controls.Add(this.label21);
            this.grpIMU.Controls.Add(this.textBox7);
            this.grpIMU.Controls.Add(this.label22);
            this.grpIMU.Controls.Add(this.textBox3);
            this.grpIMU.Controls.Add(this.label16);
            this.grpIMU.Controls.Add(this.textBox4);
            this.grpIMU.Controls.Add(this.textBox1);
            this.grpIMU.Controls.Add(this.label14);
            this.grpIMU.Controls.Add(this.textBox2);
            this.grpIMU.Controls.Add(this.label13);
            this.grpIMU.Controls.Add(this.label15);
            this.grpIMU.Location = new System.Drawing.Point(12, 232);
            this.grpIMU.Name = "grpIMU";
            this.grpIMU.Size = new System.Drawing.Size(298, 104);
            this.grpIMU.TabIndex = 51;
            this.grpIMU.TabStop = false;
            this.grpIMU.Text = "IMU";
            // 
            // textBox6
            // 
            this.textBox6.Location = new System.Drawing.Point(101, 75);
            this.textBox6.Name = "textBox6";
            this.textBox6.Size = new System.Drawing.Size(62, 20);
            this.textBox6.TabIndex = 44;
            this.textBox6.Tag = "axisReverseZ";
            this.textBox6.Text = "1";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(166, 79);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(66, 13);
            this.label21.TabIndex = 46;
            this.label21.Text = "axisSwapXY";
            // 
            // textBox7
            // 
            this.textBox7.Location = new System.Drawing.Point(233, 75);
            this.textBox7.Name = "textBox7";
            this.textBox7.Size = new System.Drawing.Size(62, 20);
            this.textBox7.TabIndex = 45;
            this.textBox7.Tag = "axisSwapXY";
            this.textBox7.Text = "0";
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(6, 79);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(72, 13);
            this.label22.TabIndex = 47;
            this.label22.Text = "axisReverseZ";
            // 
            // attitude
            // 
            this.attitude.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.attitude.Location = new System.Drawing.Point(396, 26);
            this.attitude.Name = "attitude";
            this.attitude.Size = new System.Drawing.Size(196, 196);
            this.attitude.TabIndex = 52;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::VRGimbalGUI.Properties.Resources.VR_logo;
            this.pictureBox1.Location = new System.Drawing.Point(312, 142);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(80, 74);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 53;
            this.pictureBox1.TabStop = false;
            // 
            // statusBar
            // 
            this.statusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1,
            this.sblblVersion});
            this.statusBar.Location = new System.Drawing.Point(0, 544);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(792, 22);
            this.statusBar.TabIndex = 54;
            this.statusBar.Text = "statusStrip1";
            // 
            // menuMain
            // 
            this.menuMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.menuHelp});
            this.menuMain.Location = new System.Drawing.Point(0, 0);
            this.menuMain.Name = "menuMain";
            this.menuMain.Size = new System.Drawing.Size(792, 24);
            this.menuMain.TabIndex = 55;
            this.menuMain.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuLoadConfig,
            this.menuSaveConfig,
            this.toolStripSeparator1,
            this.menuExit});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // menuLoadConfig
            // 
            this.menuLoadConfig.Name = "menuLoadConfig";
            this.menuLoadConfig.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.menuLoadConfig.Size = new System.Drawing.Size(200, 22);
            this.menuLoadConfig.Text = "L&oad config...";
            this.menuLoadConfig.Click += new System.EventHandler(this.menuLoadConfig_Click);
            // 
            // menuSaveConfig
            // 
            this.menuSaveConfig.Name = "menuSaveConfig";
            this.menuSaveConfig.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.menuSaveConfig.Size = new System.Drawing.Size(200, 22);
            this.menuSaveConfig.Text = "&Save config...";
            this.menuSaveConfig.Click += new System.EventHandler(this.menuSaveConfig_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(197, 6);
            // 
            // menuExit
            // 
            this.menuExit.Name = "menuExit";
            this.menuExit.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
            this.menuExit.Size = new System.Drawing.Size(200, 22);
            this.menuExit.Text = "E&xit";
            this.menuExit.Click += new System.EventHandler(this.menuExit_Click);
            // 
            // menuHelp
            // 
            this.menuHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuAbout});
            this.menuHelp.Name = "menuHelp";
            this.menuHelp.Size = new System.Drawing.Size(40, 20);
            this.menuHelp.Text = "&Help";
            // 
            // menuAbout
            // 
            this.menuAbout.Name = "menuAbout";
            this.menuAbout.Size = new System.Drawing.Size(114, 22);
            this.menuAbout.Text = "&About";
            this.menuAbout.Click += new System.EventHandler(this.menuAbout_Click);
            // 
            // diagSave
            // 
            this.diagSave.Filter = "VRGimbal config (*.vrg)|*.vrg";
            // 
            // diagOpen
            // 
            this.diagOpen.Filter = "VRGimbal config (*.vrg)|*.vrg";
            // 
            // graphAttitude
            // 
            this.graphAttitude.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.graphAttitude.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.graphAttitude.IsEnableVPan = false;
            this.graphAttitude.IsEnableVZoom = false;
            this.graphAttitude.IsEnableWheelZoom = false;
            this.graphAttitude.IsShowContextMenu = false;
            this.graphAttitude.IsShowCopyMessage = false;
            this.graphAttitude.Location = new System.Drawing.Point(312, 226);
            this.graphAttitude.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.graphAttitude.Name = "graphAttitude";
            this.graphAttitude.PanButtons = System.Windows.Forms.MouseButtons.None;
            this.graphAttitude.PanButtons2 = System.Windows.Forms.MouseButtons.None;
            this.graphAttitude.ScrollGrace = 0;
            this.graphAttitude.ScrollMaxX = 0;
            this.graphAttitude.ScrollMaxY = 0;
            this.graphAttitude.ScrollMaxY2 = 0;
            this.graphAttitude.ScrollMinX = 0;
            this.graphAttitude.ScrollMinY = 0;
            this.graphAttitude.ScrollMinY2 = 0;
            this.graphAttitude.SelectButtons = System.Windows.Forms.MouseButtons.None;
            this.graphAttitude.Size = new System.Drawing.Size(477, 200);
            this.graphAttitude.TabIndex = 56;
            this.graphAttitude.ZoomButtons = System.Windows.Forms.MouseButtons.None;
            // 
            // heading
            // 
            this.heading.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.heading.Location = new System.Drawing.Point(593, 26);
            this.heading.Name = "heading";
            this.heading.Size = new System.Drawing.Size(196, 196);
            this.heading.TabIndex = 59;
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(61, 17);
            this.toolStripStatusLabel1.Text = "FW Version";
            // 
            // sblblVersion
            // 
            this.sblblVersion.AutoSize = false;
            this.sblblVersion.BorderSides = ((System.Windows.Forms.ToolStripStatusLabelBorderSides)((((System.Windows.Forms.ToolStripStatusLabelBorderSides.Left | System.Windows.Forms.ToolStripStatusLabelBorderSides.Top)
                        | System.Windows.Forms.ToolStripStatusLabelBorderSides.Right)
                        | System.Windows.Forms.ToolStripStatusLabelBorderSides.Bottom)));
            this.sblblVersion.Name = "sblblVersion";
            this.sblblVersion.Size = new System.Drawing.Size(24, 17);
            this.sblblVersion.Tag = "vers";
            this.sblblVersion.Text = "00";
            // 
            // cmdDefaults
            // 
            this.cmdDefaults.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdDefaults.Location = new System.Drawing.Point(528, 0);
            this.cmdDefaults.Margin = new System.Windows.Forms.Padding(0);
            this.cmdDefaults.Name = "cmdDefaults";
            this.cmdDefaults.Size = new System.Drawing.Size(132, 26);
            this.cmdDefaults.TabIndex = 61;
            this.cmdDefaults.Text = "Defaults";
            this.cmdDefaults.UseVisualStyleBackColor = true;
            this.cmdDefaults.Click += new System.EventHandler(this.cmdDefaults_Click);
            // 
            // cmdLoadFlash
            // 
            this.cmdLoadFlash.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdLoadFlash.Location = new System.Drawing.Point(264, 0);
            this.cmdLoadFlash.Margin = new System.Windows.Forms.Padding(0);
            this.cmdLoadFlash.Name = "cmdLoadFlash";
            this.cmdLoadFlash.Size = new System.Drawing.Size(132, 26);
            this.cmdLoadFlash.TabIndex = 60;
            this.cmdLoadFlash.Text = "Load from Flash";
            this.cmdLoadFlash.UseVisualStyleBackColor = true;
            this.cmdLoadFlash.Click += new System.EventHandler(this.cmdLoadFlash_Click);
            // 
            // cmdGyroCal
            // 
            this.cmdGyroCal.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdGyroCal.Location = new System.Drawing.Point(660, 0);
            this.cmdGyroCal.Margin = new System.Windows.Forms.Padding(0);
            this.cmdGyroCal.Name = "cmdGyroCal";
            this.cmdGyroCal.Size = new System.Drawing.Size(132, 26);
            this.cmdGyroCal.TabIndex = 65;
            this.cmdGyroCal.Text = "GyroCal";
            this.cmdGyroCal.UseVisualStyleBackColor = true;
            this.cmdGyroCal.Click += new System.EventHandler(this.cmdGyroCal_Click);
            // 
            // cmdSaveFlash
            // 
            this.cmdSaveFlash.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdSaveFlash.Location = new System.Drawing.Point(396, 0);
            this.cmdSaveFlash.Margin = new System.Windows.Forms.Padding(0);
            this.cmdSaveFlash.Name = "cmdSaveFlash";
            this.cmdSaveFlash.Size = new System.Drawing.Size(132, 26);
            this.cmdSaveFlash.TabIndex = 64;
            this.cmdSaveFlash.Text = "Save to Flash";
            this.cmdSaveFlash.UseVisualStyleBackColor = true;
            this.cmdSaveFlash.Click += new System.EventHandler(this.cmdSaveFlash_Click);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.ColumnCount = 6;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel1.Controls.Add(this.cmdReadConfig, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.cmdGyroCal, 5, 0);
            this.tableLayoutPanel1.Controls.Add(this.cmdSendConfig, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.cmdLoadFlash, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.cmdDefaults, 4, 0);
            this.tableLayoutPanel1.Controls.Add(this.cmdSaveFlash, 3, 0);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 431);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(792, 26);
            this.tableLayoutPanel1.TabIndex = 66;
            // 
            // cboComPort
            // 
            this.cboComPort.FormattingEnabled = true;
            this.cboComPort.Location = new System.Drawing.Point(313, 32);
            this.cboComPort.Name = "cboComPort";
            this.cboComPort.Size = new System.Drawing.Size(77, 21);
            this.cboComPort.TabIndex = 67;
            // 
            // txtBaudRate
            // 
            this.txtBaudRate.Location = new System.Drawing.Point(313, 56);
            this.txtBaudRate.Name = "txtBaudRate";
            this.txtBaudRate.Size = new System.Drawing.Size(73, 20);
            this.txtBaudRate.TabIndex = 68;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(792, 566);
            this.Controls.Add(this.txtBaudRate);
            this.Controls.Add(this.cboComPort);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.heading);
            this.Controls.Add(this.graphAttitude);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.menuMain);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.attitude);
            this.Controls.Add(this.grpIMU);
            this.Controls.Add(this.grpAttitude);
            this.Controls.Add(this.grpPitch);
            this.Controls.Add(this.grpRoll);
            this.Controls.Add(this.cmdSM);
            this.Controls.Add(this.cmdSP);
            this.Controls.Add(this.cmdSE);
            this.Controls.Add(this.cmdSR);
            this.Controls.Add(this.cmdTC);
            this.Controls.Add(this.txtSend);
            this.Controls.Add(this.cmdSend);
            this.Controls.Add(this.txtLog);
            this.Controls.Add(this.cmdClose);
            this.Controls.Add(this.cmdOpen);
            this.MainMenuStrip = this.menuMain;
            this.MinimumSize = new System.Drawing.Size(800, 600);
            this.Name = "frmMain";
            this.Text = "VRGimbal GUI - ALPHA - www.virtualrobotix.com";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
            this.grpRoll.ResumeLayout(false);
            this.grpRoll.PerformLayout();
            this.grpPitch.ResumeLayout(false);
            this.grpPitch.PerformLayout();
            this.grpAttitude.ResumeLayout(false);
            this.grpAttitude.PerformLayout();
            this.grpIMU.ResumeLayout(false);
            this.grpIMU.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.statusBar.ResumeLayout(false);
            this.statusBar.PerformLayout();
            this.menuMain.ResumeLayout(false);
            this.menuMain.PerformLayout();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort gimbalCom;
        private System.Windows.Forms.Timer timerCom;
        private System.Windows.Forms.Button cmdOpen;
        private System.Windows.Forms.Button cmdClose;
        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.Button cmdSend;
        private System.Windows.Forms.TextBox txtSend;
        private System.Windows.Forms.Button cmdTC;
        private System.Windows.Forms.Button cmdSR;
        private System.Windows.Forms.TextBox txtRoll_KD;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtRoll_KI;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtRoll_KP;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtRoll_PWM;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtPitch_PWM;
        private System.Windows.Forms.Button cmdSE;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtPitch_KP;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtPitch_KI;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtPitch_KD;
        private System.Windows.Forms.Button cmdSP;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox txtRollMotor;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtRollDir;
        private System.Windows.Forms.Button cmdSM;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox txtPitchMotor;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox txtPitchDir;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.Button cmdSendConfig;
        private System.Windows.Forms.Button cmdReadConfig;
        private System.Windows.Forms.GroupBox grpRoll;
        private System.Windows.Forms.GroupBox grpPitch;
        private System.Windows.Forms.CheckBox chkOutputACC;
        private System.Windows.Forms.GroupBox grpAttitude;
        private System.Windows.Forms.TextBox txtYaw;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.TextBox txtPitch;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox txtRoll;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.GroupBox grpIMU;
        private AvionicsInstrumentControlDemo.AttitudeIndicatorInstrumentControl attitude;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.StatusStrip statusBar;
        private System.Windows.Forms.MenuStrip menuMain;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuExit;
        private System.Windows.Forms.ToolStripMenuItem menuLoadConfig;
        private System.Windows.Forms.ToolStripMenuItem menuSaveConfig;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem menuHelp;
        private System.Windows.Forms.ToolStripMenuItem menuAbout;
        private System.Windows.Forms.SaveFileDialog diagSave;
        private System.Windows.Forms.OpenFileDialog diagOpen;
        private ZedGraph.ZedGraphControl graphAttitude;
        private System.Windows.Forms.TextBox textBox6;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.TextBox textBox7;
        private System.Windows.Forms.Label label22;
        private AvionicsInstrumentControlDemo.HeadingIndicatorInstrumentControl heading;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.ToolStripStatusLabel sblblVersion;
        private System.Windows.Forms.Button cmdDefaults;
        private System.Windows.Forms.Button cmdLoadFlash;
        private System.Windows.Forms.Button cmdGyroCal;
        private System.Windows.Forms.Button cmdSaveFlash;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.ComboBox cboComPort;
        private System.Windows.Forms.TextBox txtBaudRate;
    }
}

