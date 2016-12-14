inherit kde
need-kde 3

IUSE="python ruby"
LICENSE="GPL-2"
KEYWORDS="~x86"
DESCRIPTION="the eXtensible Python pErl Ruby scripTable MUD client"
SRC_URI="mirror://sourceforge/xpertmud/xpertmud-3.1preview1.tar.bz2"
HOMEPAGE="http://xpertmud.sourceforge.net/"

DEPEND="${DEPEND}
        >=sys-devel/libperl-5.6.1"

RDEPEND="${RDEPEND}
		 python? ( >=dev-lang/python-2.2 )
		 ruby? ( >=dev-lang/ruby-1.8.0 )"

S="${WORKDIR}/xpertmud-3.1preview1"
src_compile() {

			econf `use_with python python` \
			      `use_with ruby ruby` || die
	        emake || die
}
