using NLog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using YamlDotNet.Dynamic;
using System.Diagnostics;
using System.Threading;
using LittleQuant.Core;

namespace LittleQuant.WinformClient
{
    public partial class MainForm : Form
    {
        private static Logger Logger = LogManager.GetCurrentClassLogger();
        private IStrategy _strategy;
        private dynamic _appConfig;

        public MainForm()
        {
            InitializeComponent();

            this.openFileDialog1.InitialDirectory = Application.StartupPath;

            using (var reader = new StreamReader(Path.Combine(Path.GetDirectoryName(this.GetType().Assembly.Location), "app.yaml")))
            {
                this._appConfig = new DynamicYaml(reader);
            }

            ExchangeManager.Log += this.ShowLog;
        }

        private void ShowLog(string log)
        {
            Action act = () => this.txtLogs.AppendText($"{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff")} : {log}" + Environment.NewLine);
            if (this.txtLogs.InvokeRequired)
                this.txtLogs.Invoke(act);
            else
                act();
        }

        private void btnLogin_Click(object sender, EventArgs e)
        {
            Task.Run(() =>
            {
                try
                {
                    ExchangeManager.InitExchanges((List<string>)this._appConfig.exchanges);
                    this.ShowLog("所有交易接口初始化完毕！");
                }
                catch (Exception ex)
                {
                    this.ShowLog("初始化交易接口失败: " + ex.GetBaseException().Message);
                }
            });
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (this.openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                try
                {
                    this._strategy = StrategyManager.Load(this.openFileDialog1.FileName);
                    this._strategy.Log += (_1, _2) => this.ShowLog(_1.ToString() + " " + _2);
                    this.ShowLog($"策略“{this._strategy.Name}”成功加载");
                    this.label5.Text = "当前策略：" + this._strategy.GetType().Name;
                }
                catch (Exception ex)
                {
                    this.ShowLog(ex.GetBaseException().Message);
                }
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            this.ShowLog(ExchangeManager.GetExchange<IOptionExchange>().Account.ToString());
        }

        private void button6_Click(object sender, EventArgs e)
        {
            this._strategy.Stop();
        }

        private void button12_Click(object sender, EventArgs e)
        {
            this._strategy.Start();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this._strategy != null)
                this._strategy.Stop();
        }
    }
}
