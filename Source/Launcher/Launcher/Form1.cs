using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace Launcher
{
    public partial class Form1 : Form
    {
        bool nonNumberEntered = false;
        int lastStringLenght = 0;

        public Form1()
        {
            InitializeComponent();
            comboBox_resolution.SelectedIndex = comboBox_resolution.Items.Count-1;
            fh = new FileHandler();
        }

        private void button_Launch_Click(object sender, EventArgs e)
        {
            IPAddress IP;
            if(!IPAddress.TryParse(textBox_IP.Text,out IP))
            {
                MessageBox.Show("Invalid IP", "Error");

            }
            else if (textBox_Tag.Text.Length < 3)
            {
                MessageBox.Show("Gamer Tag must be atleast 3 characters.", "Error");
            }
            else
            {
                fh.Save(textBox_Tag.Text, textBox_IP.Text, comboBox_resolution.Text);
                //Process.Start(@"C:\Users\Mathias\Documents\GitHub\GameProject\GameProject\Debug\GameProject.exe");
            }
        }

        private void textBox_IP_TextChanged(object sender, EventArgs e)
        {
            int lastDot = -1;

            if (lastStringLenght <= textBox_IP.TextLength)
            {
                for (int i = 0; i < textBox_IP.TextLength; i++)
                {
                    if (textBox_IP.Text[i] == '.')
                    {
                        lastDot = i;
                    }
                }
                if (textBox_IP.TextLength - lastDot > 3)
                {
                    textBox_IP.Text += '.';
                    textBox_IP.SelectionStart = textBox_IP.TextLength;
                    textBox_IP.SelectionLength = 0;
                }
            }
            lastStringLenght = textBox_IP.TextLength;
        }

        private void textBox_IP_KeyDown(object sender, KeyEventArgs e)
        {
            nonNumberEntered = false;

            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                nonNumberEntered = true;
            }
            if (Control.ModifierKeys == Keys.Shift)
            {
                nonNumberEntered = true;
            }

            if (e.KeyCode == Keys.Back)
            {
                nonNumberEntered = false;
            }
        }

        private void textBox_IP_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (nonNumberEntered == true)
            {
                e.Handled = true;
            }
        }

        private void comboBox_resolution_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
