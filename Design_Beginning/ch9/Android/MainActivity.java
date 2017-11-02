

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.cndi.www.realiotlib.*;


public class MainActivity extends AppCompatActivity {
    private static final String TAG = "SensorQuery";
    final static int SERVER_SELECT = 2001;
    final static int SERVER_PORT = 50003;

    final static String PREF_FILE_NAME = "ServerInfo";
    final static String PREF_KEY_SERVERIP = "ServerIp";
    final static String PREF_KEY_LOGINID = "LOGINID";
    final static String PREF_KEY_LOGIN_PWD = "LoginPwd";
    SharedPreferences prefs;

    final static String SERVER_DEFAULT_IP= "192.168.10.42";
    int loginID = 1111;
    String loginPwd = "reg1111";


    String ServerIP;

    final static int MAX_PACKET_BUFF = 100;
    byte[] packet;

    NetManager NetMgr;

    Packetform packetform;
    TextView NetStatus;
    PackObj packobj;
    private boolean logined = false;

    boolean connectingActionDoneFlag = false;

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        packetform = new Packetform();
        packobj = new PackObj();

        NetMgr = new NetManager();
        NetMgr.setRxHandler(mNetHandler);

        packet = new byte[MAX_PACKET_BUFF];

        // get server ip , port
        prefs = getSharedPreferences(PREF_FILE_NAME, MODE_PRIVATE);
        ServerIP = prefs.getString(PREF_KEY_SERVERIP, SERVER_DEFAULT_IP);
        loginID = prefs.getInt(PREF_KEY_LOGINID, loginID);
        loginPwd = prefs.getString(PREF_KEY_LOGIN_PWD, loginPwd);

        Log.d(TAG, "ServerIP:" + ServerIP);
        Log.d(TAG, "ServerPort:" + SERVER_PORT);
        Log.d(TAG, "loginID:" + loginID);
        Log.d(TAG, "loginPwd:" + loginPwd);

        NetStatus = (TextView) findViewById(R.id.textViewNetState);

        Button btn = (Button) findViewById(R.id.buttonServiceStart);
        btn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                btnEnable(false);

                NetMgr.setIpAndPort(ServerIP, SERVER_PORT);
                NetMgr.startThread();

                connectingActionDoneFlag = true;
            }
        });

        btn = (Button) findViewById(R.id.buttonServerSet);
        btn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                serverSel();
            }
        });
        /* 버튼에 대해서 이벤트를 넣는 부분*/

    }
    /* 버튼에 대해서 수행해야 할 함수 정의 */
    // 서버로부터 정보 수신시 호출 되는 함수
    public void rxSensorValue(int base_id, int cont_id, int object_id, int obj_type, int ent_num ,int entityId_1, int entityValue_1 ,int entityId_2, int entityValue_2 )
    {
    }

    public void serverSel() {
        Intent intent = new Intent(this, ServerSetActivity.class);

        intent.putExtra(ServerSetActivity.SERVER_IP, ServerIP);
        intent.putExtra(ServerSetActivity.LOGINID, loginID);
        intent.putExtra(ServerSetActivity.LOGIN_PWD, loginPwd);
        startActivityForResult(intent, SERVER_SELECT);
    }

    private byte ToUnSignedByte(int value) {
        int nTemp;
        if (value > 127) {
            nTemp = value - 256;
        } else
            nTemp = value;

        return (byte) nTemp;
    }

    private int unSignedByteToInt(byte value) {
        int nTemp;
        if (value > 0)
            nTemp = (int) value;
        else
            nTemp = (int) value + 256;
        return nTemp;
    }

    private void doRxCmd(Bundle data) {
        int len = data.getInt(NetManager.RX_LENGHT);
        byte[] dataArr = data.getByteArray(NetManager.RX_DATA);
        if (!packetform.isValidPktForm(dataArr, len)) {
            Log.i(TAG, "isValidPktForm error");
            return;
        }

        PackObj obj = packetform.getParcingPkt(dataArr, len);
        Log.i(TAG, "detail cmd:" + obj.cmdDetail);
        switch (obj.cmdDetail) {
            case PackObj.CMD_DETAIL_LOGIN_FAIL_RES:
                Toast.makeText(this, "Login fail !!!", Toast.LENGTH_LONG);
                NetMgr.stopThread();
                btnEnable(true);
                break;

            case PackObj.CMD_DETAIL_LOGIN_SUCCESS_RES:
                if (obj.login_id != loginID) {
                    Log.e(TAG, "Login Id 가 틀림- Tx id:" + loginID + " Rx id:" + obj.login_id);
                    NetMgr.stopThread();
                    return;
                }
                NetStatus.setText("로그인 됨");
                logined = true;
//                SendBaseIdQuery();
                break;
            case PackObj.CMD_DETAIL_BASE_ID_RES:
                break;
            case PackObj.CMD_DETAIL_CONT_ID_RES:
                break;
            case PackObj.CMD_DETAIL_OBJ_ID_RES:
                break;
            case PackObj.CMD_DETAIL_OBJ_ENT_RES:
                rxSensorValue(obj.base_id,obj.cont_id, obj.object_id,obj.obj_type, obj.ent_num ,obj.realEntityObj[0].entityId,
                        obj.realEntityObj[0].entityValue ,obj.realEntityObj[1].entityId, obj.realEntityObj[1].entityValue );
                break;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // TODO Auto-generated method stub
        if (requestCode == SERVER_SELECT) {
            if (resultCode == RESULT_OK) {
                ServerIP = data.getStringExtra(ServerSetActivity.SERVER_IP);

                Log.d(TAG, "setting Cloud Server IP:" + ServerIP);
                loginID = data.getIntExtra(ServerSetActivity.LOGINID, 1000);
                loginPwd = data.getStringExtra(ServerSetActivity.LOGIN_PWD);

                // save
                SharedPreferences.Editor ed = prefs.edit();
                ed.putString(PREF_KEY_SERVERIP, ServerIP);
                ed.putInt(PREF_KEY_LOGINID, loginID);
                ed.putString(PREF_KEY_LOGIN_PWD, loginPwd);

                ed.commit();
            }
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    private Handler mNetHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            // TODO Auto-generated method stub
            super.handleMessage(msg);

            switch (msg.what) {
                case NetManager.HANDLE_RXCMD:
                    doRxCmd(msg.getData());
                    break;
                case NetManager.HANDLE_NETSTATUS:
                    doNetStatus(msg.arg1);
                    break;
            }
        }
    };

    void btnEnable(boolean enable) {
        Button btnSetServer = (Button) findViewById(R.id.buttonServerSet);
        Button btnServiceStart = (Button) findViewById(R.id.buttonServiceStart);
        if (enable) {
            btnSetServer.setClickable(true);
            btnSetServer.setEnabled(true);

            btnServiceStart.setClickable(true);
            btnServiceStart.setEnabled(true);
        } else {
            btnSetServer.setClickable(false);
            btnSetServer.setEnabled(false);

            btnServiceStart.setClickable(false);
            btnServiceStart.setEnabled(false);
        }
    }



    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        Log.d(TAG, "OnDestroy");
        if (connectingActionDoneFlag) {
            NetMgr.stopThread();
        }
        super.onDestroy();
    }


    private void doNetStatus(int status) {
        switch (status) {
            case NetManager.NET_NONE:
                NetStatus.setText("네트워크 상태 모름");
                break;
            case NetManager.NET_DISCONNECT:
                NetStatus.setText("연결 해제");
                break;
            case NetManager.NET_CONNECTING:
                NetStatus.setText("연결 중 ...");
                break;
            case NetManager.NET_CONNECTED:
                if (!logined) {
                    NetStatus.setText("연결 후 로그인 중 ...");
                    Log.i(TAG, "sendLoginCmd start");
                    SendLoginCmd();
                }
                break;
        }
    }

//////////////////////////////////////////////////////////////////////
// Command  Send funtions
/////////////////////////////////////////////////////////////////////

    public int SendLoginCmd() {
        byte[] packet;
        packet = packetform.makeLoginPkt(loginID, loginPwd);
        return NetMgr.SendData(packet, packet.length);
    }

    public int SendBaseIdQuery() {
        byte[] packet;
        packet = packetform.makeQueryPkt(0, 0, 0, loginID);// base id query
        return NetMgr.SendData(packet, packet.length);
    }

    public int SendControlIDQuery(int baseId) {
        byte[] packet;
        packet = packetform.makeQueryPkt(baseId, 0, 0, loginID);
        return NetMgr.SendData(packet, packet.length);
    }

    public int SendObjIdQuery(int baseId, int controlId) {
        byte[] packet;
        packet = packetform.makeQueryPkt(baseId, controlId, 0, loginID);
        return NetMgr.SendData(packet, packet.length);
    }

    public int SendEntQuery(int baseId, int controlId, int objId) {
        byte[] packet;
        packet = packetform.makeQueryPkt(baseId, controlId, objId, loginID);
        return NetMgr.SendData(packet, packet.length);
    }

    public int SendEntControl(int baseId, int controlId, int objId, int obj_type, int ent_num, int entId1, int entvalue1, int entId2, int entvalue2) {
        byte[] packet;
        packet = packetform.makeActuatorControlPkt(baseId, controlId, objId, obj_type, ent_num, entId1, entvalue1, entId2, entvalue2);
        return NetMgr.SendData(packet, packet.length);
    }

    public int UpdateDataCmd(int baseid, int controlid, int objid) {
        byte[] packet;

        packet = packetform.makeQueryPkt(baseid, controlid, objid, loginID);
        return NetMgr.SendData(packet, packet.length);
    }

}
