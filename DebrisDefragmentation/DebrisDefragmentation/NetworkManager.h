#pragma once

struct DDPacketHeader;

class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	void Init();
	void Connect();

	void SendAcceleration();
	void SendStop();
	void SendRotation( double y, double x );

	int GetMyPlayerId() const { return m_MyPlayerId; }
	void SetMyPlayerId( int val ) { m_MyPlayerId = val; }

	void RegisterHandles();

	static void HandleLoginResult( DDPacketHeader& pktBase );
	static void HandleAccelerationResult( DDPacketHeader& pktBase );
	static void HandleStopResult( DDPacketHeader& pktBase );
	static void HandleRotationResult( DDPacketHeader& pktBase );
	static void HandleSyncResult( DDPacketHeader& pktBase );

private:
	static int m_MyPlayerId;
	
};

extern NetworkManager* GNetworkManager;