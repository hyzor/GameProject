namespace Launcher
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tp_join = new System.Windows.Forms.TabPage();
            this.panel1 = new System.Windows.Forms.Panel();
            this.textBox_IP = new System.Windows.Forms.TextBox();
            this.textBox_Tag = new System.Windows.Forms.TextBox();
            this.label_IP = new System.Windows.Forms.Label();
            this.label_Tag = new System.Windows.Forms.Label();
            this.button_Launch = new System.Windows.Forms.Button();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.tp_create = new System.Windows.Forms.TabPage();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.comboBox_resolution = new System.Windows.Forms.ComboBox();
            this.tabControl1.SuspendLayout();
            this.tp_join.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.tp_create.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tp_join);
            this.tabControl1.Controls.Add(this.tp_create);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(458, 252);
            this.tabControl1.TabIndex = 0;
            // 
            // tp_join
            // 
            this.tp_join.BackColor = System.Drawing.Color.Transparent;
            this.tp_join.Controls.Add(this.comboBox_resolution);
            this.tp_join.Controls.Add(this.panel1);
            this.tp_join.Controls.Add(this.button_Launch);
            this.tp_join.Controls.Add(this.pictureBox2);
            this.tp_join.Location = new System.Drawing.Point(4, 22);
            this.tp_join.Name = "tp_join";
            this.tp_join.Padding = new System.Windows.Forms.Padding(3);
            this.tp_join.Size = new System.Drawing.Size(450, 226);
            this.tp_join.TabIndex = 0;
            this.tp_join.Text = "Join Game";
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(50)))), ((int)(((byte)(50)))));
            this.panel1.Controls.Add(this.textBox_IP);
            this.panel1.Controls.Add(this.textBox_Tag);
            this.panel1.Controls.Add(this.label_IP);
            this.panel1.Controls.Add(this.label_Tag);
            this.panel1.Location = new System.Drawing.Point(8, 88);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(194, 114);
            this.panel1.TabIndex = 6;
            // 
            // textBox_IP
            // 
            this.textBox_IP.AutoCompleteCustomSource.AddRange(new string[] {
            "127.0.0.1"});
            this.textBox_IP.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.textBox_IP.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
            this.textBox_IP.Location = new System.Drawing.Point(6, 77);
            this.textBox_IP.Name = "textBox_IP";
            this.textBox_IP.Size = new System.Drawing.Size(182, 20);
            this.textBox_IP.TabIndex = 4;
            this.textBox_IP.Text = "127.0.0.1";
            this.textBox_IP.TextChanged += new System.EventHandler(this.textBox_IP_TextChanged);
            this.textBox_IP.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_IP_KeyDown);
            this.textBox_IP.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_IP_KeyPress);
            // 
            // textBox_Tag
            // 
            this.textBox_Tag.Location = new System.Drawing.Point(6, 27);
            this.textBox_Tag.Name = "textBox_Tag";
            this.textBox_Tag.Size = new System.Drawing.Size(182, 20);
            this.textBox_Tag.TabIndex = 1;
            this.textBox_Tag.Text = "adadsdsd";
            // 
            // label_IP
            // 
            this.label_IP.AutoSize = true;
            this.label_IP.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_IP.ForeColor = System.Drawing.Color.White;
            this.label_IP.Location = new System.Drawing.Point(3, 61);
            this.label_IP.Name = "label_IP";
            this.label_IP.Size = new System.Drawing.Size(60, 13);
            this.label_IP.TabIndex = 1;
            this.label_IP.Text = "Server IP";
            // 
            // label_Tag
            // 
            this.label_Tag.AutoSize = true;
            this.label_Tag.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Tag.ForeColor = System.Drawing.Color.White;
            this.label_Tag.Location = new System.Drawing.Point(3, 11);
            this.label_Tag.Name = "label_Tag";
            this.label_Tag.Size = new System.Drawing.Size(69, 13);
            this.label_Tag.TabIndex = 3;
            this.label_Tag.Text = "Gamer Tag";
            // 
            // button_Launch
            // 
            this.button_Launch.BackColor = System.Drawing.Color.Silver;
            this.button_Launch.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_Launch.Location = new System.Drawing.Point(319, 178);
            this.button_Launch.Name = "button_Launch";
            this.button_Launch.Size = new System.Drawing.Size(119, 24);
            this.button_Launch.TabIndex = 3;
            this.button_Launch.Text = "Launch Game";
            this.button_Launch.UseVisualStyleBackColor = false;
            this.button_Launch.Click += new System.EventHandler(this.button_Launch_Click);
            // 
            // pictureBox2
            // 
            this.pictureBox2.Image = global::Launcher.Properties.Resources.x4YBVpy;
            this.pictureBox2.Location = new System.Drawing.Point(0, 0);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(450, 223);
            this.pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox2.TabIndex = 0;
            this.pictureBox2.TabStop = false;
            // 
            // tp_create
            // 
            this.tp_create.BackColor = System.Drawing.Color.Transparent;
            this.tp_create.Controls.Add(this.pictureBox1);
            this.tp_create.Location = new System.Drawing.Point(4, 22);
            this.tp_create.Name = "tp_create";
            this.tp_create.Padding = new System.Windows.Forms.Padding(3);
            this.tp_create.Size = new System.Drawing.Size(450, 226);
            this.tp_create.TabIndex = 1;
            this.tp_create.Text = "Create Server";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::Launcher.Properties.Resources.ghostship_8bbo2dnl;
            this.pictureBox1.InitialImage = ((System.Drawing.Image)(resources.GetObject("pictureBox1.InitialImage")));
            this.pictureBox1.Location = new System.Drawing.Point(-1, 0);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(451, 223);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // comboBox_resolution
            // 
            this.comboBox_resolution.BackColor = System.Drawing.Color.Silver;
            this.comboBox_resolution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_resolution.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.comboBox_resolution.FormattingEnabled = true;
            this.comboBox_resolution.Items.AddRange(new object[] {
            "800 x 600",
            "1024 x 768 ",
            "1280 x 800",
            "1600 x 900",
            "1680 x 1050",
            "1920 x 1080"});
            this.comboBox_resolution.Location = new System.Drawing.Point(8, 61);
            this.comboBox_resolution.Name = "comboBox_resolution";
            this.comboBox_resolution.Size = new System.Drawing.Size(137, 21);
            this.comboBox_resolution.TabIndex = 4;
            this.comboBox_resolution.SelectedIndexChanged += new System.EventHandler(this.comboBox_resolution_SelectedIndexChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.ClientSize = new System.Drawing.Size(454, 246);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.ShowIcon = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "Launcher";
            this.tabControl1.ResumeLayout(false);
            this.tp_join.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.tp_create.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tp_join;
        private System.Windows.Forms.TabPage tp_create;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox textBox_Tag;
        private System.Windows.Forms.Label label_IP;
        private System.Windows.Forms.Label label_Tag;
        private System.Windows.Forms.Button button_Launch;
        private System.Windows.Forms.PictureBox pictureBox2;
        private FileHandler fh;
        private System.Windows.Forms.TextBox textBox_IP;
        private System.Windows.Forms.ComboBox comboBox_resolution;
    }
}

