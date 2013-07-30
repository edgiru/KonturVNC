#include "KonturRemoteViewerCore.h"
#include "util/AnsiStringStorage.h"


KonturRemoteViewerCore::KonturRemoteViewerCore(Logger *logger)
	:RemoteViewerCore(logger),m_avilog(0)
{}

KonturRemoteViewerCore::KonturRemoteViewerCore(RfbInputGate *input, RfbOutputGate *output,
                   CoreEventsAdapter *adapter, Logger *logger, bool sharedFlag)
				   :RemoteViewerCore(input, output, adapter, logger, sharedFlag),m_avilog(0)
{}

KonturRemoteViewerCore::KonturRemoteViewerCore(const TCHAR *host, UINT16 port,
                   CoreEventsAdapter *adapter, Logger *logger, bool sharedFlag)
				   :RemoteViewerCore(host, port, adapter, logger, sharedFlag),m_avilog(0)
{}

KonturRemoteViewerCore::KonturRemoteViewerCore(SocketIPv4 *socket,
                   CoreEventsAdapter *adapter, Logger *logger, bool sharedFlag)
				   :RemoteViewerCore(socket, adapter, logger, sharedFlag),m_avilog(0)
{}

KonturRemoteViewerCore::~KonturRemoteViewerCore()
{
	if(m_avilog)
	{
		m_avilog->terminate();
		m_avilog->wait();
	}
}

void KonturRemoteViewerCore::setID(const StringStorage *id)
{
  m_id = *id;
}

StringStorage KonturRemoteViewerCore::getProtocolString() const
{
  StringStorage protocolString;
  protocolString.format(_T("RFB %03d.%03d\n"), 3, 8);
  return protocolString;
}


void KonturRemoteViewerCore::handshake()
{
  char serverProtocol[13];
  char repeaterHandshake[250];
  serverProtocol[12] = 0;

  m_input->readFully(serverProtocol, 12);
  memset(&repeaterHandshake,0,250);
  StringStorage repeater;
  repeater.format(_T("ID:%s"), m_id.getString());
  AnsiStringStorage ansiRepeater(&repeater);
  memcpy(&repeaterHandshake,ansiRepeater.getString(),ansiRepeater.getLength());
  m_output->writeFully(&repeaterHandshake,250);

  AnsiStringStorage clientProtocolAnsi;
  clientProtocolAnsi.fromStringStorage(&getProtocolString());
  m_output->writeFully(clientProtocolAnsi.getString(), 12);
  m_output->flush();
}


void KonturRemoteViewerCore::setFbProperties(const Dimension *fbDimension,
                       const PixelFormat *fbPixelFormat)
{
	RemoteViewerCore::setFbProperties(fbDimension, fbPixelFormat);
	if(!m_avilog)
	{
		m_avilog = new AvilogThread(&m_frameBuffer);
		m_avilog->resume();
	}
	else
	{
		m_avilog->UpdateAvilog(&m_frameBuffer);
	}

}